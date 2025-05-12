#include "../include/statement.h"

#include <memory>

#include "../include/expression.h"

ReturnStatement::ReturnStatement(std::unique_ptr<Expression> expr)
    : expr(std::move(expr)) {}

FunCallStatement::FunCallStatement(
    std::string name, std::vector<std::unique_ptr<Expression>> args)
    : name(std::move(name)), args(std::move(args)) {}

AssignmentStatement::AssignmentStatement(std::unique_ptr<AccessExpr> access_expr,
                                         std::unique_ptr<Expression> expr)
    : access_expr(std::move(access_expr)), expr(std::move(expr)) {}

WriteStatement::WriteStatement(std::unique_ptr<Expression> expr)
    : expr(std::move(expr)) {}

ReadStatement::ReadStatement(std::unique_ptr<AccessExpr> access_expr)
    : access_expr(std::move(access_expr)) {}

WhileStatement::WhileStatement(std::unique_ptr<Expression> condition,
                               std::unique_ptr<Statement> body)
    : condition(std::move(condition)), body(std::move(body)) {}

IfStatement::IfStatement(std::unique_ptr<Expression> condition,
                         std::unique_ptr<Statement> trueBranch,
                         std::unique_ptr<Statement> falseBranch)
    : condition(std::move(condition)),
      trueBranch(std::move(trueBranch)),
      falseBranch(std::move(falseBranch)) {}

VarDeclStatement::VarDeclStatement(std::string varName, std::unique_ptr<Type> type)
    : varName(std::move(varName)), type(std::move(type)) {}

SeqStatement::SeqStatement(std::unique_ptr<Statement> first,
                           std::unique_ptr<Statement> second)
    : first(std::move(first)), second(std::move(second)) {}

Definition::Definition(std::string name, std::vector<std::pair<std::string, std::unique_ptr<Type>>> args,
                       std::unique_ptr<Statement> body)
    : name(std::move(name)), args(std::move(args)), body(std::move(body)) {}

ProgramValue::ProgramValue(std::vector<llvm::Function *> definitions)
    : definitions(std::move(definitions)) {}

Program::Program(std::vector<std::unique_ptr<Definition>> definitions,
                 std::unique_ptr<Statement> body)
    : definitions(std::move(definitions)), body(std::move(body)) {}

void ProgramValue::print() {
  for (const auto &definition : definitions) {
    definition->print(llvm::errs());
  }
}
