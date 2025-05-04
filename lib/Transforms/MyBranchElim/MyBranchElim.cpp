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
#include "llvm/Transforms/Utils/BasicBlockUtils.h"


#include  <iostream>

using namespace llvm;

PreservedAnalyses MyBranchElim::run(Function &F, FunctionAnalysisManager &FAM) {

    std::cout << "MyBranchElim in function: " << F.getName().str() << std::endl;

    bool changed = false;

    // write smth
    SmallVector<std::pair<BasicBlock*, BasicBlock*>, 4> Merges;

    for (auto &BB : F) {
        for (auto II = BB.begin(), IE = BB.end(); II != IE; ) {
            Instruction *Inst = &*II++;
            if (auto *Branch = dyn_cast<BranchInst>(Inst)) {
                if (Branch->isConditional()) {
                    if (auto *CondVal = dyn_cast<ConstantInt>(Branch->getCondition())) {

                        BasicBlock *TargetBB = CondVal->isOne() ? Branch->getSuccessor(0) : Branch->getSuccessor(1);

                        BranchInst *NewBranch = BranchInst::Create(TargetBB);
                        ReplaceInstWithInst(Branch, NewBranch);

                        if (TargetBB->hasNPredecessors(1)) {
                            Merges.push_back({ &BB, TargetBB });
                        }
                        changed = true;
                    }
                } else {
                    BasicBlock *TargetBB = Branch->getSuccessor(0);

                    if (TargetBB->hasNPredecessors(1)) {
                        Merges.push_back({ &BB, TargetBB });
                        changed = true;
                    }
                }
            }
        }
    }

    for (auto BB = F.begin(), E = F.end(); BB != E; ) {
        BasicBlock *CurrBB = &*BB++;

        if (CurrBB == &F.getEntryBlock())
            continue;

        if (CurrBB->hasNPredecessors(0)) {
            changed = true;

            for (auto *SuccBB : successors(CurrBB)) {
                for (auto &PI : SuccBB->phis())
                    PI.removeIncomingValue(CurrBB, true);
            }

            CurrBB->eraseFromParent();
        }
    }

    for (auto &Pair : Merges) {
        BasicBlock *SourceBB = Pair.first;
        BasicBlock *TargetBB = Pair.second;

        auto merged = MergeBlockIntoPredecessor(TargetBB);

        assert(merged && ("Didn't merge " + TargetBB->getName().str() + " into " + SourceBB->getName().str()).data());
    }

    if (!changed) {
        return PreservedAnalyses::all();
    }
    return PreservedAnalyses::none();
}