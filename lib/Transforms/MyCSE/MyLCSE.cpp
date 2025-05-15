// Copyright (c) 2025. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by aleksandr on 15.05.25.
//

#include "MyLCSE.h"
#include <algorithm>

llvm::MyLCSE::MyLCSE(ExprBindings previous_expr) : previous_expr(std::move(previous_expr)) {}

bool llvm::isExpression(Instruction& Inst) {
  return isa<BinaryOperator>(Inst) ||
         isa<CmpInst>(Inst) ||
         isa<UnaryOperator>(Inst) ||
         isa<CastInst>(Inst) ||
         isa<LoadInst>(Inst) ||
         isa<GetElementPtrInst>(Inst) ||
         isa<SelectInst>(Inst);
}

bool llvm::similar(Instruction* inst1, Instruction* inst2) {
  if (inst1->getOpcode() != inst2->getOpcode()) return false;

  if (inst1->getType() != inst2->getType()) return false;

  if (isa<BinaryOperator>(inst1) && isa<BinaryOperator>(inst2)) {
    return inst1->getOperand(0) == inst2->getOperand(0) &&
     inst1->getOperand(1) == inst2->getOperand(1);
  }

  if (isa<CmpInst>(inst1) && isa<CmpInst>(inst2)) {
    return inst1->getOperand(0) == inst2->getOperand(0) &&
     inst1->getOperand(1) == inst2->getOperand(1);
  }

  return false;
}

llvm::ExprBindings llvm::MyLCSE::run(BasicBlock* bb) {

  auto expr_bindings = previous_expr;

  std::vector<Instruction*> toErase;
  for (auto &inst : *bb) {
    if (isExpression(inst)) {
      // Canonicalize the instruction for matching
      canonicalize(inst);

      auto it = std::find_if(expr_bindings.begin(), expr_bindings.end(),
        [&inst](Instruction* expr) {
          return similar(expr, &inst);
        });
      if (it != expr_bindings.end()) {
        // Found a common subexpression, replace uses
        inst.replaceAllUsesWith(*it);
        toErase.push_back(&inst);
      } else {
        expr_bindings.push_back(&inst);
      }
    }
  }
  // Remove redundant instructions
  for (auto inst : toErase) {
    inst->eraseFromParent();
  }
  return expr_bindings;
}

void llvm::canonicalize(Instruction& op) {
  if (auto *binOp = dyn_cast<BinaryOperator>(&op)) {
    if (binOp->isCommutative()) {
      Value *op0 = binOp->getOperand(0);
      Value *op1 = binOp->getOperand(1);
      if (op1 < op0) {
        binOp->setOperand(0, op1);
        binOp->setOperand(1, op0);
      }
    }
  }
}