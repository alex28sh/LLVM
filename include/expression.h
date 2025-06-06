#pragma once

#include <memory>
#include <string>
#include <vector>

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

enum class BinOp { Sub, Add, Div, Mul, Mod, Eq, Neq, Gt, Ge, Lt, Le, And, Or };

enum class UnOp { SubUn };

class Type {
public:
  virtual ~Type() = default;

  virtual llvm::Value *gen_alloc(std::string name) = 0;
};

class IntType : public Type {
public:

  llvm::Value *gen_alloc(std::string name) override;
};

class PtrType : public Type {
public:
  explicit PtrType(std::unique_ptr<Type> baseType);

  llvm::Value *gen_alloc(std::string name) override;

private:
  std::unique_ptr<Type> baseType;
};

class Expression {
 public:
  virtual ~Expression() = default;

  virtual llvm::Value *codegen() = 0;
};

class BinExpr : public Expression {
 public:
  BinExpr(BinOp op, std::unique_ptr<Expression> left,
          std::unique_ptr<Expression> right);

  llvm::Value *codegen() override;

 private:
  BinOp op;
  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;
};

class UnExpr : public Expression {
 public:
  UnExpr(UnOp op, std::unique_ptr<Expression> expr);

  llvm::Value *codegen() override;

 private:
  UnOp op;
  std::unique_ptr<Expression> expr;
};

class FunCall : public Expression {
 public:
  FunCall(std::string name, std::vector<std::unique_ptr<Expression>> args);

  llvm::Value *codegen() override;

 private:
  std::string name;
  std::vector<std::unique_ptr<Expression>> args;
};


class AccessExpr : public Expression {
};

class Variable : public AccessExpr {
 public:
  explicit Variable(std::string name, bool load, bool is_ptr);

  llvm::Value *codegen() override;

 private:
  std::string name;
  bool load;
  bool is_ptr;
};

class ArrayIdx : public AccessExpr {
 public:
  ArrayIdx(std::unique_ptr<AccessExpr> access_expr, std::unique_ptr<Expression> index, bool load, bool is_ptr);

  llvm::Value *codegen() override;

 private:
  std::unique_ptr<AccessExpr> access_expr;
  std::unique_ptr<Expression> index;
  bool load;
  bool is_ptr;
};

class Const : public Expression {
 public:
  explicit Const(double value);

  llvm::Value *codegen() override;

 private:
  double value;
};

class NewArr : public Expression {
 public:
  explicit NewArr(std::unique_ptr<Type> type, std::unique_ptr<Expression> size);

  llvm::Value *codegen() override;

 private:
  std::unique_ptr<Type> type;
  std::unique_ptr<Expression> size;
};
