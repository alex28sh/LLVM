#include <iostream>

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
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

PreservedAnalyses MyBranchElim::run(Function &F, FunctionAnalysisManager &) {
  std::cout << "MyBranchElim in function: " << F.getName().str() << std::endl;

  bool changed = false;

  // write smth
  SmallVector<std::pair<BasicBlock *, BasicBlock *>, 4> merges;

  for (auto &bb : F) {
    for (auto ii = bb.begin(), ie = bb.end(); ii != ie;) {
      Instruction *inst = &*ii++;
      if (auto *branch = dyn_cast<BranchInst>(inst)) {
        if (branch->isConditional()) {
          if (const auto *cond_val = dyn_cast<ConstantInt>(branch->getCondition())) {
            BasicBlock *target_bb = cond_val->isOne() ? branch->getSuccessor(0)
                                                    : branch->getSuccessor(1);

            BranchInst *new_branch = BranchInst::Create(target_bb);
            ReplaceInstWithInst(branch, new_branch);

            if (target_bb->hasNPredecessors(1)) {
              merges.push_back({&bb, target_bb});
            }
            changed = true;
          }
        } else {
          if (BasicBlock *target_bb = branch->getSuccessor(0);
              target_bb->hasNPredecessors(1)) {
            merges.push_back({&bb, target_bb});
            changed = true;
          }
        }
      }
    }
  }

  for (auto bb = F.begin(), e = F.end(); bb != e;) {
    BasicBlock *curr_bb = &*bb++;

    if (curr_bb == &F.getEntryBlock()) continue;

    if (curr_bb->hasNPredecessors(0)) {
      changed = true;

      for (auto *succ_bb : successors(curr_bb)) {
        for (auto &pi : succ_bb->phis()) pi.removeIncomingValue(curr_bb, true);
      }

      curr_bb->eraseFromParent();
    }
  }

  for (auto &[fst, snd] : merges) {
    const BasicBlock *source_bb = fst;
    BasicBlock *target_bb = snd;

    const auto merged = MergeBlockIntoPredecessor(target_bb);

    assert(merged && ("Didn't merge " + target_bb->getName().str() + " into " +
                      source_bb->getName().str())
                         .data());
  }

  if (!changed) {
    return PreservedAnalyses::all();
  }
  return PreservedAnalyses::none();
}