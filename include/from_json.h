#pragma once

#include <memory>
#include <nlohmann/json.hpp>

#include "./expression.h"
#include "./statement.h"

using json = nlohmann::json;

std::unique_ptr<Expression> expr_from_json(json j);

std::unique_ptr<Statement> stmt_from_json(json j);

std::unique_ptr<Definition> def_from_json(json j);

std::unique_ptr<Program> prg_from_json(json j);
