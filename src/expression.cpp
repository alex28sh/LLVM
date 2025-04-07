#include <memory>
#include <utility>
#include <vector>
#include <map>

#include "../include/expression.h"


using namespace llvm;

BinExpr::BinExpr(BinOp op, std::unique_ptr<Expression> &left, std::unique_ptr<Expression> &right)
    : op(op),
      left(std::move(left)),
      right(std::move(right)) {
}

UnExpr::UnExpr(UnOp op, std::unique_ptr<Expression> &expr)
    : op(op),
      expr(std::move(expr)) {
}

FunCall::FunCall(std::string name, std::vector<std::unique_ptr<Expression>> args) :
    name(std::move(name)),
    args(std::move(args)) {
}

Variable::Variable(std::string name) : name(std::move(name)) {}

Const::Const(double value)
    : value(value) {
}

