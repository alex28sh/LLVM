// Copyright (c) 2025. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by aleksandr on 15.05.25.
//

#include <iostream>


#include "MyLCSE.h"
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
#include "llvm/ADT/GraphTraits.h" // Add this include

using namespace llvm;

PreservedAnalyses MyCSE::run(Function &F, FunctionAnalysisManager &AM) {
  std::cout << "MyCSE in function: " << F.getName().str() << std::endl;

  DominatorTree dt(F);

  // Collect basic blocks in dominator tree topological (pre-order) order
  std::vector<BasicBlock*> domOrder;
  auto *rootNode = dt.getRootNode();
  if (rootNode) {
    std::function<void(DomTreeNodeBase<BasicBlock>*)> preorder =
      [&](DomTreeNodeBase<BasicBlock>* node) {
        domOrder.push_back(node->getBlock());
        for (auto *child : node->children())
          preorder(child);
      };
    preorder(rootNode);
  }

  // Example: print the order
  std::cerr << "Dominator tree topological order:" << std::endl;
  for (auto *bb : domOrder) {
    std::cerr << bb->getName().str() << std::endl;
  }

  std::unordered_map<BasicBlock*, ExprBindings> memoized_expr;

  auto changed = std::move(domOrder);
  while (!changed.empty()) {
    auto new_changed = std::vector<BasicBlock*>();
    for (auto bb : changed) {
      auto parent = dt.getNode(bb)->getIDom();
      auto available = std::vector<Instruction*>();
      if (parent) {
        available = memoized_expr[parent->getBlock()];
      }
      auto lcse = std::make_unique<MyLCSE>(available);
      auto computed = lcse->run(bb);
      if (memoized_expr[bb] != computed) {
        memoized_expr[bb] = computed;
        for (auto next : dt.getNode(bb)->children()) {
          new_changed.push_back(next->getBlock());
        }
      }
    }
    changed = std::move(new_changed);
  }

  return PreservedAnalyses::none();
}

