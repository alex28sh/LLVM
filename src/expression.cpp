#include "../include/expression.h"

#include <map>
#include <memory>
#include <utility>
#include <vector>

using std::unique_ptr;

PtrType::PtrType(unique_ptr<Type> baseType)
    : baseType(std::move(baseType)) {}

BinExpr::BinExpr(BinOp op, unique_ptr<Expression> left,
                 unique_ptr<Expression> right)
    : op(op), left(std::move(left)), right(std::move(right)) {}

UnExpr::UnExpr(UnOp op, unique_ptr<Expression> expr)
    : op(op), expr(std::move(expr)) {}

FunCall::FunCall(std::string name, std::vector<unique_ptr<Expression>> args)
    : name(std::move(name)), args(std::move(args)) {}

Variable::Variable(std::string name, bool load, bool is_ptr) : name(std::move(name)), load(load), is_ptr(is_ptr) {}

ArrayIdx::ArrayIdx(unique_ptr<AccessExpr> access_expr, unique_ptr<Expression> index, bool load, bool is_ptr)
    : access_expr(std::move(access_expr)), index(std::move(index)), load(load), is_ptr(is_ptr) {}

Const::Const(double value) : value(value) {}

NewArr::NewArr(std::unique_ptr<Type> type, std::unique_ptr<Expression> size) :
        type(std::move(type)), size(std::move(size)) {}
