#include "../include/generate_llvm.h"

#include <iostream>

#include "../include/expression.h"
#include "../include/statement.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Type.h"

std::map<std::string, llvm::Value *> NamedValues;

llvm::Constant *formatStrConstant;
llvm::ArrayType *formatStrType;
llvm::GlobalVariable *formatStrGlobal;

llvm::Value *BinExpr::codegen() {
  llvm::Value *l = left->codegen();
  llvm::Value *r = right->codegen();
  if (!l || !r) return nullptr;

  std::cerr << "BinExpr codegen\n";
  l->getType()->print(llvm::errs());
  r->getType()->print(llvm::errs());
  llvm::errs().flush();
  std::cerr << "\n";
  std::cerr.flush();
  assert(l->getType() == r->getType());
  switch (op) {
    case BinOp::Add:
      return builder->CreateAdd(l, r, "addtmp");
    case BinOp::Sub:
      return builder->CreateSub(l, r, "subtmp");
    case BinOp::Mul:
      return builder->CreateMul(l, r, "multmp");
    case BinOp::Lt:
      l = builder->CreateICmpSLT(l, r, "cmptmp");
      return builder->CreateZExt(l, llvm::Type::getInt64Ty(*the_context),
                               "booltmp");
    case BinOp::Gt:
      l = builder->CreateICmpSGT(l, r, "cmptmp");
      return builder->CreateZExt(l, llvm::Type::getInt64Ty(*the_context),
                                 "booltmp");
    case BinOp::Eq:
      l = builder->CreateICmpEQ(l, r, "cmptmp");
      return builder->CreateZExt(l, llvm::Type::getInt64Ty(*the_context),
                                 "booltmp");
    case BinOp::Neq:
      l = builder->CreateICmpNE(l, r, "cmptmp");
      return builder->CreateZExt(l, llvm::Type::getInt64Ty(*the_context),
                                 "booltmp");
    case BinOp::Le:
      l = builder->CreateICmpSLE(l, r, "cmptmp");
      return builder->CreateZExt(l, llvm::Type::getInt64Ty(*the_context),
                                 "booltmp");
    case BinOp::Ge:
      l = builder->CreateICmpSGE(l, r, "cmptmp");
      return builder->CreateZExt(l, llvm::Type::getInt64Ty(*the_context),
                                 "booltmp");
    case BinOp::Div:
      return builder->CreateSDiv(l, r, "divtmp");
    case BinOp::Mod:
      return builder->CreateSRem(l, r, "modtmp");
    case BinOp::And:
      return builder->CreateAnd(l, r, "andtmp");
    case BinOp::Or:
      return builder->CreateOr(l, r, "ortmp");
    default:
      throw std::runtime_error("Not implemented binary op");
  }
}

llvm::Value *UnExpr::codegen() {
  llvm::Value *expr_val = expr->codegen();
  switch (op) {
    case UnOp::SubUn:
      return builder->CreateNeg(expr_val, "neg");
    default:
      throw std::runtime_error("Not implemented unary op");
  }
}

llvm::Value *FunCall::codegen() {
  llvm::Function *callee_f = the_module->getFunction(name);

  if (!callee_f) throw std::runtime_error("Unknown function referenced");

  if (callee_f->arg_size() != args.size())
    throw std::runtime_error("Incorrect # arguments passed");

  std::vector<llvm::Value *> args_v;
  for (unsigned i = 0, e = args.size(); i != e; ++i) {
    args_v.push_back(args[i]->codegen());
    if (!args_v.back()) return nullptr;
  }

  return builder->CreateCall(callee_f, args_v, "calltmp");
}

llvm::Value *Variable::codegen() {
  NamedValues[name]->getType()->print(llvm::errs());
  llvm::errs().flush();
  if (!load) {
    return NamedValues[name];
  }
  llvm::Type* ptr_type = llvm::PointerType::get(*the_context, 0);
  llvm::Type* int_type = llvm::Type::getInt64Ty(*the_context);
  return builder->CreateLoad(is_ptr ? ptr_type : int_type, NamedValues[name], name + "_loaded");
}

llvm::Value *ArrayIdx::codegen() {
  llvm::Value *array = access_expr->codegen();
  llvm::Value *index_value = index->codegen();

  if (!array || !index_value) return nullptr;

  llvm::Value *idx = builder->CreateGEP(
    llvm::Type::getInt64Ty(*the_context), array, index_value, "arrayidx");
  if (!load) {
    return idx;
  }
  llvm::Type* ptr_type = llvm::PointerType::get(*the_context, 0);
  llvm::Type* int_type = llvm::Type::getInt64Ty(*the_context);
  return builder->CreateLoad(is_ptr ? ptr_type : int_type, idx, "arrayidx_loaded");
}

llvm::Value *Const::codegen() {
  return llvm::ConstantInt::get(*the_context,
                                llvm::APInt(64, static_cast<uint64_t>(value)));
}

llvm::Value *IntType::gen_alloc(std::string name) {
  std::cerr << "Allocating int\n";
  std::cerr << name << "\n";
  std::cerr.flush();
  return builder->CreateAlloca(llvm::Type::getInt64Ty(*the_context), nullptr, name);
}

llvm::Value *PtrType::gen_alloc(std::string name) {
  std::cerr << "Allocating PtrType\n";
  std::cerr << name << "\n";
  std::cerr.flush();
  // llvm::Type *i64_type = llvm::Type::getInt64Ty(*the_context);
  llvm::PointerType *pointer_type = llvm::PointerType::get(*the_context, 0);
  assert(pointer_type->isPointerTy());
  return builder->CreateAlloca(pointer_type, nullptr, name);
}

llvm::Value *NewArr::codegen() {

  llvm::Value *size_val = size->codegen();

  // Declare or get malloc function (returns i8* and takes one i64 argument)
  llvm::FunctionType *malloc_type = llvm::FunctionType::get(
    llvm::PointerType::get(llvm::Type::getInt8Ty(*the_context), 0),
    {llvm::Type::getInt64Ty(*the_context)},
    false);
  auto *malloc_func = llvm::cast<llvm::Function>(
the_module->getOrInsertFunction("malloc", malloc_type).getCallee());

  // Compute allocation size: element count * size of int (assume 4 bytes for int)
  llvm::Value *element_size = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*the_context), 8);
  llvm::Value *alloc_size = builder->CreateMul(size_val, element_size, "allocSize");

  // Call malloc to allocate memory
  llvm::Value *raw_ptr = builder->CreateCall(malloc_func, alloc_size, "raw_ptr");

  // Convert the i8* result of malloc to int* (i32*)
  llvm::Value *int_ptr = builder->CreateBitCast(
    raw_ptr, llvm::PointerType::get(llvm::Type::getInt64Ty(*the_context), 0), "int_ptr");

  // 'intPtr' now represents the result of new int[n]
  return int_ptr;
}


llvm::Value *ReturnStatement::codegen() { return expr->codegen(); }

llvm::Value *FunCallStatement::codegen() {
  llvm::Function *callee_f = the_module->getFunction(name);

  if (!callee_f) throw std::runtime_error("Unknown function referenced");

  if (callee_f->arg_size() != args.size())
    throw std::runtime_error("Incorrect # arguments passed");

  std::vector<llvm::Value *> args_v;
  for (unsigned i = 0, e = args.size(); i != e; ++i) {
    args_v.push_back(args[i]->codegen());
    if (!args_v.back()) return nullptr;
  }

  builder->CreateCall(callee_f, args_v, "calltmp");
  return nullptr;
}

llvm::Value *AssignmentStatement::codegen() {
  builder->CreateStore(expr->codegen(), access_expr->codegen());
  return nullptr;
}

llvm::Value *WriteStatement::codegen() {
  llvm::Value *expr_val = expr->codegen();
  if (!expr_val) return nullptr;

  std::vector<llvm::Type *> arg_types = {formatStrGlobal->getType()};

  llvm::FunctionType *print_type =
      llvm::FunctionType::get(builder->getInt64Ty(), arg_types, true);
  llvm::FunctionCallee print_func =
      the_module->getOrInsertFunction("printf", print_type);

  if (!print_func) throw std::runtime_error("Unknown function 'printf'");

  builder->CreateCall(print_func, {formatStrGlobal, expr_val}, "printfcall");
  return nullptr;
}

llvm::Value *ReadStatement::codegen() {
  llvm::Value *var = access_expr->codegen();

  if (!var) return nullptr;

  llvm::Value *var_ptr = var;

  std::vector<llvm::Type *> arg_types = {formatStrGlobal->getType()};

  llvm::FunctionType *scanf_type =
      llvm::FunctionType::get(builder->getInt64Ty(), arg_types, true);
  llvm::FunctionCallee scanf_func =
      the_module->getOrInsertFunction("__isoc99_scanf", scanf_type);
  if (!scanf_func) throw std::runtime_error("Unknown function '__isoc99_scanf'");

  llvm::ArrayRef<llvm::Value *> args = {formatStrGlobal, var_ptr};

  builder->CreateCall(scanf_func, args, "scanfcall");
  return nullptr;
}

llvm::Value *WhileStatement::codegen() {
  llvm::Function *the_function = builder->GetInsertBlock()->getParent();
  llvm::BasicBlock *preheader_bb = builder->GetInsertBlock();
  llvm::BasicBlock *cond_bb =
      llvm::BasicBlock::Create(*the_context, "cond", the_function);

  builder->CreateBr(cond_bb);
  builder->SetInsertPoint(cond_bb);

  llvm::Value *cond = condition->codegen();
  if (!cond) return nullptr;
  cond = builder->CreateICmpNE(
      cond, llvm::ConstantInt::get(*the_context, llvm::APInt(64, 0)),
      "loopcond");

  llvm::BasicBlock *after_bb =
      llvm::BasicBlock::Create(*the_context, "afterloop", the_function);

  llvm::BasicBlock *body_bb =
      llvm::BasicBlock::Create(*the_context, "body", the_function);

  builder->CreateCondBr(cond, body_bb, after_bb);
  builder->SetInsertPoint(body_bb);

  body->codegen();

  llvm::Value *cond_end = condition->codegen();
  if (!cond_end) return nullptr;
  cond_end = builder->CreateICmpNE(
      cond_end, llvm::ConstantInt::get(*the_context, llvm::APInt(64, 0)),
      "loopcond");
  builder->CreateCondBr(cond_end, body_bb, after_bb);
  builder->SetInsertPoint(after_bb);

  return nullptr;
}

llvm::Value *IfStatement::codegen() {
  llvm::Value *cond_v = condition->codegen();
  if (!cond_v) return nullptr;

  cond_v = builder->CreateICmpNE(
      cond_v, llvm::ConstantInt::get(*the_context, llvm::APInt(64, 0)), "ifcond");

  llvm::Function *the_function = builder->GetInsertBlock()->getParent();

  llvm::BasicBlock *then_bb =
      llvm::BasicBlock::Create(*the_context, "then", the_function);
  llvm::BasicBlock *else_bb = llvm::BasicBlock::Create(*the_context, "else");
  llvm::BasicBlock *merge_bb = llvm::BasicBlock::Create(*the_context, "ifcont");

  builder->CreateCondBr(cond_v, then_bb, else_bb);

  builder->SetInsertPoint(then_bb);

  llvm::Value *then_v = trueBranch->codegen();

  builder->CreateBr(merge_bb);
  then_bb = builder->GetInsertBlock();

  the_function->insert(the_function->end(), else_bb);
  builder->SetInsertPoint(else_bb);

  llvm::Value *else_v = falseBranch->codegen();

  builder->CreateBr(merge_bb);
  else_bb = builder->GetInsertBlock();

  the_function->insert(the_function->end(), merge_bb);
  builder->SetInsertPoint(merge_bb);

  if (then_v && else_v) {
    llvm::PHINode *pn =
        builder->CreatePHI(llvm::Type::getInt64Ty(*the_context), 2, "iftmp");

    pn->addIncoming(then_v, then_bb);
    pn->addIncoming(else_v, else_bb);
    return pn;
  }
  return nullptr;
}

llvm::Value *VarDeclStatement::codegen() {
  // type->gen_alloc(varName)->getType()->print(llvm::errs());
  NamedValues[varName] = type->gen_alloc(varName);
  // NamedValues[varName] = builder->CreateAlloca(
  //     llvm::Type::getInt64Ty(*the_context), nullptr, varName);
  return nullptr;
}

llvm::Value *SeqStatement::codegen() {
  llvm::Value *first_val = first->codegen();

  llvm::Value *second_val = second->codegen();
  return second_val;
}

llvm::Value *SkipStatement::codegen() { return nullptr; }

llvm::Function *Definition::codegen() {
  std::vector<llvm::Type *> integers(args.size(),
                                     llvm::Type::getInt64Ty(*the_context));

  llvm::FunctionType *ft = llvm::FunctionType::get(
      llvm::Type::getInt64Ty(*the_context), integers, false);

  llvm::Function *f = llvm::Function::Create(
      ft, llvm::Function::ExternalLinkage, name, the_module.get());

  unsigned idx = 0;
  for (auto &arg : f->args()) arg.setName(args[idx++].first + "_ARG");

  llvm::Function *the_function = the_module->getFunction(name);

  if (!the_function) the_function = f;

  if (!the_function) return nullptr;

  llvm::BasicBlock *bb =
      llvm::BasicBlock::Create(*the_context, "entry", the_function);
  builder->SetInsertPoint(bb);

  NamedValues.clear();

  idx = 0;
  for (auto &arg : the_function->args()) {
    NamedValues[args[idx].first] = args[idx].second->gen_alloc(args[idx].first);
    // NamedValues[args[idx].first] = builder->CreateAlloca(
    //     llvm::Type::getInt64Ty(*the_context), nullptr, args[idx]);
    idx++;
  }

  idx = 0;
  for (auto &arg : the_function->args()) {
    builder->CreateStore(&arg, NamedValues[args[idx].first]);
    idx++;
  }

  if (llvm::Value *ret_val = body->codegen()) {
    builder->CreateRet(ret_val);

    llvm::verifyFunction(*the_function);

    the_fpm->run(*the_function, *the_fam);

    return the_function;
  }

  the_function->eraseFromParent();
  return nullptr;
}

void Program::constgen() {
  if (!the_module) {
    throw std::runtime_error("failed to load module constgen");
  }

  formatStrConstant =
      llvm::ConstantDataArray::getString(*the_context, "%ld", true);
  formatStrType = llvm::ArrayType::get(llvm::Type::getInt8Ty(*the_context), 4);
  formatStrGlobal = new llvm::GlobalVariable(*the_module, formatStrType, true,
                                             llvm::GlobalValue::PrivateLinkage,
                                             formatStrConstant, ".str");
  formatStrGlobal->setUnnamedAddr(llvm::GlobalValue::UnnamedAddr::Global);
  formatStrGlobal->setAlignment(llvm::MaybeAlign(1));
}

ProgramValue *Program::codegen() {
  constgen();

  std::vector<llvm::Function *> definitions_compiled;
  definitions_compiled.reserve(definitions.size() + 1);
  for (const auto &definition : definitions) {
    definitions_compiled.push_back(definition->codegen());
  }
  auto main_def = std::make_unique<Definition>(
      "main", std::vector<std::pair<std::string, std::unique_ptr<Type>>>(), std::move(body));
  definitions_compiled.push_back(main_def->codegen());
  return new ProgramValue(definitions_compiled);
}
