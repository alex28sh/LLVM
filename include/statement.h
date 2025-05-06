#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../include/expression.h"

class Statement {
 public:
  virtual ~Statement() = default;

  virtual llvm::Value *codegen() = 0;
};

class ReturnStatement : public Statement {
 public:
  explicit ReturnStatement(std::unique_ptr<Expression> expr);

  llvm::Value *codegen() override;

 private:
  std::unique_ptr<Expression> expr;
};

class FunCallStatement : public Statement {
 public:
  FunCallStatement(std::string name,
                   std::vector<std::unique_ptr<Expression>> args);

  llvm::Value *codegen() override;

 private:
  std::string name;
  std::vector<std::unique_ptr<Expression>> args;
};

class AssignmentStatement : public Statement {
 public:
  AssignmentStatement(std::string varName, std::unique_ptr<Expression> expr);

  llvm::Value *codegen() override;

 private:
  std::string varName;
  std::unique_ptr<Expression> expr;
};

class WriteStatement : public Statement {
 public:
  explicit WriteStatement(std::unique_ptr<Expression> expr);

  llvm::Value *codegen() override;

 private:
  std::unique_ptr<Expression> expr;
};

class ReadStatement : public Statement {
 public:
  explicit ReadStatement(std::string varName);

  llvm::Value *codegen() override;

 private:
  std::string varName;
};

class WhileStatement : public Statement {
 public:
  WhileStatement(std::unique_ptr<Expression> condition,
                 std::unique_ptr<Statement> body);

  llvm::Value *codegen() override;

 private:
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Statement> body;
};

class IfStatement : public Statement {
 public:
  IfStatement(std::unique_ptr<Expression> condition,
              std::unique_ptr<Statement> trueBranch,
              std::unique_ptr<Statement> falseBranch);

  llvm::Value *codegen() override;

 private:
  std::unique_ptr<Expression> condition;
  std::unique_ptr<Statement> trueBranch;
  std::unique_ptr<Statement> falseBranch;
};

class VarDeclStatement : public Statement {
 public:
  explicit VarDeclStatement(std::string varName);

  llvm::Value *codegen() override;

 private:
  std::string varName;
};

class SeqStatement : public Statement {
 public:
  SeqStatement(std::unique_ptr<Statement> first,
               std::unique_ptr<Statement> second);

  llvm::Value *codegen() override;

 private:
  std::unique_ptr<Statement> first;
  std::unique_ptr<Statement> second;
};

class SkipStatement : public Statement {
  llvm::Value *codegen() override;
};

class Definition {
 public:
  Definition(std::string name, std::vector<std::string> args,
             std::unique_ptr<Statement> body);

  llvm::Function *codegen();

 private:
  std::string name;
  std::vector<std::string> args;
  std::unique_ptr<Statement> body;
};

class ProgramValue {
 public:
  explicit ProgramValue(std::vector<llvm::Function *> definitions);

  void print();

 private:
  std::vector<llvm::Function *> definitions;
};

class Program {
 public:
  Program(std::vector<std::unique_ptr<Definition>> definitions,
          std::unique_ptr<Statement> body);

  ProgramValue *codegen();

  void constgen();

 private:
  std::vector<std::unique_ptr<Definition>> definitions;
  std::unique_ptr<Statement> body;
};
