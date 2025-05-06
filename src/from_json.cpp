

#include "../include/from_json.h"

#include <iostream>

#include "../include/expression.h"

BinOp binop_from_json(json j) {
  static const std::unordered_map<std::string, BinOp> bin_ops = {
      {"Sub", BinOp::Sub}, {"Add", BinOp::Add}, {"Div", BinOp::Div},
      {"Mul", BinOp::Mul}, {"Mod", BinOp::Mod}, {"Eq", BinOp::Eq},
      {"Neq", BinOp::Neq}, {"Gt", BinOp::Gt},   {"Ge", BinOp::Ge},
      {"Lt", BinOp::Lt},   {"Le", BinOp::Le},   {"And", BinOp::And},
      {"Or", BinOp::Or}};
  auto it = bin_ops.find(j.get<std::string>());
  if (it != bin_ops.end()) {
    return it->second;
  } else {
    throw std::runtime_error("Could not find binop");
  }
}

UnOp unop_from_json(json j) {
  if (j.get<std::string>() == "SubUn") {
    return UnOp::SubUn;
  } else {
    throw std::runtime_error("Could not find unop");
  }
}

std::unique_ptr<Expression> expr_from_json(json j) {
  std::string type = j["tag"].get<std::string>();
  auto contents = j["contents"];
  if (type == "BinExpr") {
    auto l = expr_from_json(contents[1]);
    auto r = expr_from_json(contents[2]);
    return std::make_unique<BinExpr>(binop_from_json(contents[0]), std::move(l),
                                     std::move(r));
  }
  if (type == "UnExpr") {
    auto e = expr_from_json(contents[1]);
    return std::make_unique<UnExpr>(unop_from_json(contents[0]), std::move(e));
  }
  if (type == "FunCall") {
    std::string name = contents[0].get<std::string>();
    std::vector<json> args_json = contents[1].get<std::vector<json>>();
    std::vector<std::unique_ptr<Expression>> args(args_json.size());
    std::transform(args_json.begin(), args_json.end(), args.begin(),
                   expr_from_json);
    return std::make_unique<FunCall>(name, std::move(args));
  }
  if (type == "Variable") {
    std::string var = contents.get<std::string>();
    return std::make_unique<Variable>(var);
  }
  if (type == "Const") {
    double x = contents.get<double>();
    return std::make_unique<Const>(x);
  }
  throw std::runtime_error("Could not create expression");
}

std::unique_ptr<Statement> stmt_from_json(json j) {
  std::string type = j["tag"].get<std::string>();
  auto contents = j["contents"];
  if (type == "ReturnStmt") {
    auto e = expr_from_json(contents);
    return std::make_unique<ReturnStatement>(std::move(e));
  }
  if (type == "FunCallStmt") {
    std::string name = contents[0].get<std::string>();
    std::vector<json> args_json = contents[1].get<std::vector<json>>();
    std::vector<std::unique_ptr<Expression>> args(args_json.size());
    std::transform(args_json.begin(), args_json.end(), args.begin(),
                   expr_from_json);
    return std::make_unique<FunCallStatement>(name, std::move(args));
  }
  if (type == "Assignment") {
    std::string name = contents[0].get<std::string>();
    auto e = expr_from_json(contents[1]);
    return std::make_unique<AssignmentStatement>(name, std::move(e));
  }
  if (type == "Write") {
    auto e = expr_from_json(contents);
    return std::make_unique<WriteStatement>(std::move(e));
  }
  if (type == "Read") {
    std::string name = contents.get<std::string>();
    return std::make_unique<ReadStatement>(name);
  }
  if (type == "While") {
    auto e = expr_from_json(contents[0]);
    auto stmt = stmt_from_json(contents[1]);
    return std::make_unique<WhileStatement>(std::move(e), std::move(stmt));
  }
  if (type == "If") {
    auto e = expr_from_json(contents[0]);
    auto l = stmt_from_json(contents[1]);
    auto r = stmt_from_json(contents[2]);
    return std::make_unique<IfStatement>(std::move(e), std::move(l),
                                         std::move(r));
  }
  if (type == "VarDecl") {
    std::string name = contents.get<std::string>();
    return std::make_unique<VarDeclStatement>(name);
  }
  if (type == "SeqStmt") {
    auto l = stmt_from_json(contents[0]);
    auto r = stmt_from_json(contents[1]);
    return std::make_unique<SeqStatement>(std::move(l), std::move(r));
  }
  if (type == "Skip") {
    return std::make_unique<SkipStatement>();
  }
  throw std::runtime_error("Could not create statement");
}

std::unique_ptr<Definition> def_from_json(json j) {
  std::string name = j[0].get<std::string>();
  std::vector<std::string> args = j[1].get<std::vector<std::string>>();
  std::unique_ptr<Statement> stmt = stmt_from_json(j[2]);
  return std::make_unique<Definition>(name, args, std::move(stmt));
}

std::unique_ptr<Program> prg_from_json(json j) {
  std::vector<json> defs_json = j[0].get<std::vector<json>>();
  std::vector<std::unique_ptr<Definition>> defs(defs_json.size());
  std::transform(defs_json.begin(), defs_json.end(), defs.begin(),
                 def_from_json);

  std::unique_ptr<Statement> stmt = stmt_from_json(j[1]);
  return std::make_unique<Program>(std::move(defs), std::move(stmt));
}
