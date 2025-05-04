#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Constants.h"
#include "llvm/Transforms/Utils/PromoteMemToReg.h"
#include "llvm/IR/Dominators.h"  // Include for DominatorTree
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/ConstantFold.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Analysis/ConstantFolding.h"

#include  <iostream>

using namespace llvm;

PreservedAnalyses MyConstantProp::run(Function &F, FunctionAnalysisManager &FAM) {

    std::cout << "MyConstantProp in function: " << F.getName().str() << std::endl;

    BasicBlock &Entry = F.getEntryBlock();
    SmallVector<AllocaInst*, 16> Allocas;
    for (Instruction &I : Entry) {
        if (auto *AI = dyn_cast<AllocaInst>(&I))
            Allocas.push_back(AI);
    }

    // If any allocas are found, run mem2reg.
    if (!Allocas.empty()) {
        DominatorTree DT(F);
        errs() << "Promoting " << Allocas.size() << " allocas using mem2reg\n";

        // PromoteMemoryToRegister transforms the allocas to SSA form.
        PromoteMemToReg(Allocas, DT);
    }

    const DataLayout &DL = F.getParent()->getDataLayout();

    std::vector<Instruction *> workList;
    for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
            // We'll only consider instructions that do not have side effects and are not terminators.
            if (!I.mayHaveSideEffects() && !I.isTerminator())
                workList.push_back(&I);
        }
    }

    bool changed = false;

    // Process the worklist until no more instructions are eligible.
    while (!workList.empty()) {
        Instruction *Inst = workList.back();
        workList.pop_back();

        // Check if the instruction is still in the IR. It might have been erased already.
        if (!Inst->getParent())
            continue;

        // Attempt to constant fold the instruction.
        if (Constant *C = ConstantFoldInstruction(Inst, DL)) {
            // Replace all uses of the instruction with the folded constant.
            Inst->replaceAllUsesWith(C);
            changed = true;

            // For every instruction that uses the constant result, add them to the worklist.
            for (User *U : C->users()) {
                if (Instruction *UserInst = dyn_cast<Instruction>(U))
                    workList.push_back(UserInst);
            }

            // Erase the instruction from its parent.
            Inst->eraseFromParent();
        }
    }

    if (!changed) {
        return PreservedAnalyses::all();
    }

    PreservedAnalyses PA;
    // PA.preserve<CFGAnalyses>();
    PA.preserve(reinterpret_cast<AnalysisKey*>(CFGAnalyses::ID()));
    return PA;
}