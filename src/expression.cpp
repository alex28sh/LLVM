#include "../include/expression.h"

#include <map>
#include <memory>
#include <utility>
#include <vector>

using std::unique_ptr;

BinExpr::BinExpr(BinOp op, unique_ptr<Expression> left,
                 unique_ptr<Expression> right)
    : op(op), left(std::move(left)), right(std::move(right)) {}

UnExpr::UnExpr(UnOp op, unique_ptr<Expression> expr)
    : op(op), expr(std::move(expr)) {}

FunCall::FunCall(std::string name, std::vector<unique_ptr<Expression>> args)
    : name(std::move(name)), args(std::move(args)) {}

Variable::Variable(std::string name) : name(std::move(name)) {}

Const::Const(double value) : value(value) {}
