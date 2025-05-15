// Copyright (c) 2025. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by aleksandr on 15.05.25.
//

#ifndef MYLCSE_H
#define MYLCSE_H

#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManager.h"

namespace llvm {

  using ExprBindings = std::vector<Instruction*>;

  class MyLCSE {
  public:
    explicit MyLCSE(ExprBindings previous_expr);

    ExprBindings run(BasicBlock* bb);

    ExprBindings previous_expr;
  };

  void canonicalize(Instruction& op);

  bool isExpression(Instruction& Inst);

  bool similar(Instruction* inst1, Instruction* inst2);

} // namespace llvm

#endif //MYLCSE_H
