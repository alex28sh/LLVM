#include <iostream>

#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Dominators.h"  // Include for DominatorTree
#include "llvm/IR/Function.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/PromoteMemToReg.h"

using namespace llvm;

PreservedAnalyses MyConstantProp::run(Function &F,
                                      FunctionAnalysisManager &) {
  std::cout << "MyConstantProp in function: " << F.getName().str() << std::endl;

  BasicBlock &entry = F.getEntryBlock();
  SmallVector<AllocaInst *, 16> allocs;
  for (Instruction &i : entry) {
    if (auto *ai = llvm::dyn_cast<AllocaInst>(&i))
      if (isAllocaPromotable(ai))
        allocs.push_back(ai);
  }

  // If any allocas are found, run mem2reg.
  if (!allocs.empty()) {
    DominatorTree dt(F);
    errs() << "Promoting " << allocs.size() << " allocas using mem2reg\n";

    // PromoteMemoryToRegister transforms the allocas to SSA form.
    PromoteMemToReg(allocs, dt);
  }

  const DataLayout &dl = F.getParent()->getDataLayout();

  std::vector<Instruction *> work_list;
  for (BasicBlock &bb : F) {
    for (Instruction &i : bb) {
      // We'll only consider instructions that do not have side effects and are
      // not terminators.
      if (!i.mayHaveSideEffects() && !i.isTerminator()) work_list.push_back(&i);
    }
  }

  bool changed = false;

  // Process the worklist until no more instructions are eligible.
  while (!work_list.empty()) {
    Instruction *inst = work_list.back();
    work_list.pop_back();

    // Check if the instruction is still in the IR. It might have been erased
    // already.
    if (!inst->getParent()) continue;

    // Attempt to constant fold the instruction.
    if (Constant *C = ConstantFoldInstruction(inst, dl)) {
      // Replace all uses of the instruction with the folded constant.
      inst->replaceAllUsesWith(C);
      changed = true;

      // For every instruction that uses the constant result, add them to the
      // worklist.
      for (User *u : C->users()) {
        if (auto *user_inst = dyn_cast<Instruction>(u))
          work_list.push_back(user_inst);
      }

      // Erase the instruction from its parent.
      inst->eraseFromParent();
    }
  }

  if (!changed) {
    return PreservedAnalyses::all();
  }

  PreservedAnalyses pa;
  pa.preserve(reinterpret_cast<AnalysisKey *>(CFGAnalyses::ID()));
  return pa;
}