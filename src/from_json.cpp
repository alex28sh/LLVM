

#include "../include/from_json.h"

#include <iostream>

#include "../include/expression.h"


std::unique_ptr<AccessExpr> access_expr_from_json(json j, bool load, bool is_ptr) {
  std::string type = j["tag"].get<std::string>();
  if (type == "Variable") {
    std::string name = j["contents"].get<std::string>();
    return std::make_unique<Variable>(name, load, is_ptr);
  }
  if (type == "ArrayIdx") {
    auto access_expr = access_expr_from_json(j["contents"][0], true, true);
    auto index = expr_from_json(j["contents"][1]);
    return std::make_unique<ArrayIdx>(std::move(access_expr), std::move(index), load, is_ptr);
  }

  throw std::runtime_error("Could not create access expression");
}

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

std::unique_ptr<Type> get_type_from_json(json j) {
  std::string type = j["tag"].get<std::string>();
  if (type == "TInt") {
    return std::make_unique<IntType>();
  }
  if (type == "TPtr") {
    auto base = get_type_from_json(j["contents"]);
    return std::make_unique<PtrType>(std::move(base));
  }
  throw std::runtime_error("Could not create type");
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
  if (type == "Access") {
    // std::vector<json> args_json = contents[0].get<std::vector<json>>();
    return std::move(access_expr_from_json(contents, true, false));
  }
  if (type == "Const") {
    double x = contents.get<double>();
    return std::make_unique<Const>(x);
  }
  if (type == "NewArr") {
    auto type_arr = get_type_from_json(contents[0]);
    auto size = expr_from_json(contents[1]);
    return std::make_unique<NewArr>(std::move(type_arr), std::move(size));
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
    auto e = expr_from_json(contents[1]);
    return std::make_unique<AssignmentStatement>(std::move(access_expr_from_json(contents[0], false, false)), std::move(e));
  }
  if (type == "Write") {
    auto e = expr_from_json(contents);
    return std::make_unique<WriteStatement>(std::move(e));
  }
  if (type == "Read") {
    return std::make_unique<ReadStatement>(std::move(access_expr_from_json(contents, false, false)));
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
    auto type_json = get_type_from_json(contents[0]);
    std::string name = contents[1];
    return std::make_unique<VarDeclStatement>(name, std::move(type_json));
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

std::pair<std::string, std::unique_ptr<Type>> arg_from_json(json j) {
  auto name = j[1].get<std::string>();
  auto type_json = get_type_from_json(j[0]);
  return std::make_pair(name, std::move(type_json));
}

std::unique_ptr<Definition> def_from_json(json j) {
  std::string name = j[0].get<std::string>();
  std::vector<json> args_json = j[1].get<std::vector<json>>();
  std::vector<std::pair<std::string, std::unique_ptr<Type>>> args(args_json.size());
  std::transform(args_json.begin(), args_json.end(), args.begin(), arg_from_json);

  std::unique_ptr<Statement> stmt = stmt_from_json(j[2]);
  return std::make_unique<Definition>(name, std::move(args), std::move(stmt));
}

std::unique_ptr<Program> prg_from_json(json j) {
  std::vector<json> defs_json = j[0].get<std::vector<json>>();
  std::vector<std::unique_ptr<Definition>> defs(defs_json.size());
  std::transform(defs_json.begin(), defs_json.end(), defs.begin(),
                 def_from_json);

  std::unique_ptr<Statement> stmt = stmt_from_json(j[1]);
  return std::make_unique<Program>(std::move(defs), std::move(stmt));
}
