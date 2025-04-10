
#include "../include/expression.h"
#include "../include/statement.h"
#include "../include/generate_llvm.h"

#include <iostream>

#include "llvm/IR/GlobalVariable.h"

std::map<std::string, Value *> NamedValues;

Constant *formatStrConstant;
ArrayType *formatStrType;
GlobalVariable *formatStrGlobal;

Value* BinExpr::codegen() {
    Value *L = left->codegen();
    Value *R = right->codegen();
    if (!L || !R)
        return nullptr;

    switch (op) {
        case BinOp::Add:
            return Builder->CreateFAdd(L, R, "addtmp");
        case BinOp::Sub:
            return Builder->CreateFSub(L, R, "subtmp");
        case BinOp::Mul:
            return Builder->CreateFMul(L, R, "multmp");
        case BinOp::Lt:
            L = Builder->CreateFCmpULT(L, R, "cmptmp");
            return Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "booltmp");
        case BinOp::Eq:
            L = Builder->CreateFCmpUEQ(L, R, "cmptmp");
            return Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext), "booltmp");
        case BinOp::Le:
            L = Builder->CreateFCmpULE(L, R, "cmptmp");
            return Builder->CreateUIToFP(L, Type::getDoubleTy(*TheContext),
        default:
            throw std::runtime_error("Not implemented binary op");
    }
}

Value* UnExpr::codegen() {
    Value *Expr = expr->codegen();
    switch (op) {
        case UnOp::SubUn:
            return Builder->CreateNeg(Expr, "neg");
        default:
            throw std::runtime_error("Not implemented unary op");
    }
}

Value* FunCall::codegen() {
    Function *CalleeF = TheModule->getFunction(name);

    if (!CalleeF)
        throw std::runtime_error("Unknown function referenced");

    // If argument mismatch error.
    if (CalleeF->arg_size() != args.size())
        throw std::runtime_error("Incorrect # arguments passed");

    std::vector<Value *> ArgsV;
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
        ArgsV.push_back(args[i]->codegen());
        if (!ArgsV.back())
            return nullptr;
    }
    // CalleeF->getFunctionType()->print(errs());
    // std::cerr << CalleeF->getFunctionType()->getNumParams() << "\n";

    // for (unsigned i = 0; i != ArgsV.size(); ++i) {
    //     CalleeF->getFunctionType()->getParamType(i)->print(errs());
    //     ArgsV[i]->getType()->print(errs());
    //     // std::cerr << (i >= CalleeF->getFunctionType()->getNumParams()) << " " <<
    //     //         (CalleeF->getFunctionType()->getParamType(i) == ArgsV[i]->getType()) << " ";
    // }

    return Builder->CreateCall(CalleeF, ArgsV, "calltmp");
}

Value* Variable::codegen() {

    Value *V;
    if (NamedValues[name]->getType()->isPointerTy()) {
        // std::cerr << name << "\n";
        V = Builder->CreateLoad(Type::getDoubleTy(*TheContext), NamedValues[name], name + "_loaded");
        // V->getType()->print(errs());
    } else {
        V = NamedValues[name];
    }
    if (!V)
        throw std::runtime_error("Unknown variable name");
    return V;
}

Value* Const::codegen() {
    return ConstantFP::get(*TheContext, APFloat(value));
}


Value* ReturnStatement::codegen() {
    return expr->codegen();
}

Value* FunCallStatement::codegen() {
    Function *CalleeF = TheModule->getFunction(name);

    if (!CalleeF)
        throw std::runtime_error("Unknown function referenced");

    if (CalleeF->arg_size() != args.size())
        throw std::runtime_error("Incorrect # arguments passed");

    std::vector<Value *> ArgsV;
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
        ArgsV.push_back(args[i]->codegen());
        if (!ArgsV.back())
            return nullptr;
    }

    Builder->CreateCall(CalleeF, ArgsV, "calltmp");
    return nullptr;
}

Value* AssignmentStatement::codegen() {
    Builder->CreateStore(expr->codegen(), NamedValues[varName]);
    return nullptr;
}

Value* WriteStatement::codegen() {
    Value *ExprVal = expr->codegen();
    if (!ExprVal)
        return nullptr;

    std::vector<Type *> argTypes = {formatStrGlobal->getType()};

    FunctionType *printType = FunctionType::get(
        Builder->getInt32Ty(), argTypes, true);
    FunctionCallee PrintFunc = TheModule->getOrInsertFunction("printf", printType);

    // PrintFunc.getFunctionType()->print(errs());

    if (!PrintFunc)
        throw std::runtime_error("Unknown function 'printf'");

    Builder->CreateCall(PrintFunc, {formatStrGlobal, ExprVal}, "printfcall");
    return nullptr;
}

Value* ReadStatement::codegen() {

    Value *var = NamedValues[varName];

    if (!var)
        return nullptr;

    Value *varPtr = var; //Builder->CreateLoad(var->getType(), var, varName + "_ptr");

    std::vector<Type *> argTypes = {formatStrGlobal->getType()};

    FunctionType *scanfType = FunctionType::get(
        Builder->getInt32Ty(), argTypes, true);
    FunctionCallee ScanfFunc = TheModule->getOrInsertFunction("__isoc99_scanf", scanfType);
    if (!ScanfFunc)
        throw std::runtime_error("Unknown function '__isoc99_scanf'");
    // ScanfFunc.getFunctionType()->print(errs());

    ArrayRef <Value*> args = {formatStrGlobal, varPtr};
    // std::cerr << "\n";
    // for (unsigned i = 0; i != args.size(); ++i)
    //     std::cerr << (i >= scanfType->getNumParams() ||
    //             scanfType->getParamType(i) == args[i]->getType()) << "\n";

    Builder->CreateCall(ScanfFunc, args, "scanfcall");
    return nullptr;
}

Value* WhileStatement::codegen() {

    Function *TheFunction = Builder->GetInsertBlock()->getParent();
    BasicBlock *PreheaderBB = Builder->GetInsertBlock();
    BasicBlock *LoopBB = BasicBlock::Create(*TheContext, "loop", TheFunction);

    Builder->CreateBr(LoopBB);
    Builder->SetInsertPoint(LoopBB);

    Value *cond = condition->codegen();
    if (!cond)
        return nullptr;
    cond = Builder->CreateFCmpONE(
      cond, ConstantFP::get(*TheContext, APFloat(0.0)), "loopcond");

    BasicBlock *AfterBB =
        BasicBlock::Create(*TheContext, "afterloop", TheFunction);

    Builder->CreateCondBr(cond, LoopBB, AfterBB);

    body->codegen();

    Builder->CreateCondBr(cond, LoopBB, AfterBB);

    return nullptr;
}

Value* IfStatement::codegen() {

    Value *CondV = condition->codegen();
    if (!CondV)
        return nullptr;

    CondV = Builder->CreateFCmpONE(
        CondV, ConstantFP::get(*TheContext, APFloat(0.0)), "ifcond");

    Function *TheFunction = Builder->GetInsertBlock()->getParent();

    BasicBlock *ThenBB = BasicBlock::Create(*TheContext, "then", TheFunction);
    BasicBlock *ElseBB = BasicBlock::Create(*TheContext, "else");
    BasicBlock *MergeBB = BasicBlock::Create(*TheContext, "ifcont");

    Builder->CreateCondBr(CondV, ThenBB, ElseBB);

    Builder->SetInsertPoint(ThenBB);

    Value *ThenV = trueBranch->codegen();

    Builder->CreateBr(MergeBB);
    ThenBB = Builder->GetInsertBlock();

    TheFunction->insert(TheFunction->end(), ElseBB);
    Builder->SetInsertPoint(ElseBB);

    Value *ElseV = falseBranch->codegen();

    Builder->CreateBr(MergeBB);
    ElseBB = Builder->GetInsertBlock();

    TheFunction->insert(TheFunction->end(), MergeBB);
    Builder->SetInsertPoint(MergeBB);

    if (ThenV && ElseV) {
        PHINode *PN = Builder->CreatePHI(Type::getDoubleTy(*TheContext), 2, "iftmp");

        PN->addIncoming(ThenV, ThenBB);
        PN->addIncoming(ElseV, ElseBB);
        return PN;
    }
    return nullptr;
}

Value* VarDeclStatement::codegen() {
    NamedValues[varName] = Builder->CreateAlloca(Type::getDoubleTy(*TheContext), nullptr, varName);
    return nullptr;
}

Value* SeqStatement::codegen() {
    Value *FirstVal = first->codegen();

    Value *SecondVal = second->codegen();
    return SecondVal;
}

Value* SkipStatement::codegen() {
    // return Constant::getNullValue(Type::getDoubleTy(*TheContext));
    return nullptr;
}

Function* Definition::codegen() {
    std::vector<Type *> Doubles(args.size(), Type::getDoubleTy(*TheContext));

    FunctionType *FT =
        FunctionType::get(Type::getDoubleTy(*TheContext), Doubles, false);

    Function *F =
        Function::Create(FT, Function::ExternalLinkage, name, TheModule.get());

    unsigned Idx = 0;
    for (auto &Arg : F->args())
        Arg.setName(args[Idx++]);


    Function *TheFunction = TheModule->getFunction(name);

    if (!TheFunction)
        TheFunction = F;

    if (!TheFunction)
        return nullptr;


    BasicBlock *BB = BasicBlock::Create(*TheContext, "entry", TheFunction);
    Builder->SetInsertPoint(BB);

    NamedValues.clear();
    for (auto &Arg : TheFunction->args())
        NamedValues[std::string(Arg.getName())] = &Arg;

    if (Value *RetVal = body->codegen()) {
        Builder->CreateRet(RetVal);

        verifyFunction(*TheFunction);

        // std::cerr << "Definition " << name << " compiled\n";

        return TheFunction;
    }

    TheFunction->eraseFromParent();
    return nullptr;
}

void Program::constgen() {

    if  (!TheModule) {
        throw std::runtime_error("failed to load module constgen");
    }

    formatStrConstant = ConstantDataArray::getString(
        *TheContext,
        "%lf",
        true);
    formatStrType = ArrayType::get(Type::getInt8Ty(*TheContext), 4); // 4 = "%f\n" + '\0'
    formatStrGlobal = new GlobalVariable(
        *TheModule,
        formatStrType,
        true,                                // isConstant
        GlobalValue::PrivateLinkage,
        formatStrConstant,
        ".str"
    );
    formatStrGlobal->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
    formatStrGlobal->setAlignment(MaybeAlign(1));

}


Program_Value* Program::codegen() {
    constgen();
    // std::cerr << "constgen compiled\n";

    std::vector<Function*> definitions_compiled;
    for (const auto &definition : definitions) {
        definitions_compiled.push_back(definition->codegen());
    }
    // std::cerr << "entering main\n";
    auto main_def = std::make_unique<Definition>("main", std::vector<std::string>(), std::move(body));
    definitions_compiled.push_back(main_def->codegen());
    return new Program_Value(definitions_compiled);
}
