/*****************************************************************************
* @file plc_generator.h
*
* @brief Implements the code generator for the PLC program compiler.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_GENERATOR_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_GENERATOR_H

#include <deque>
#include <ostream>
#include <variant>

#include "plc_parser.h"

namespace plc_compiler::generator
{

using Error = struct Error;

struct Error
{
    public:
        Error(parser::SharedIdAstNode node);
        Error(parser::ExpressionAstNode node);
        friend std::ostream& operator<<(std::ostream& stream, const Error& error);
        std::variant<parser::SharedIdAstNode, parser::ExpressionAstNode> node;
};

bool GenerateCode(parser::SharedStatementAstNode root, std::ostream &stream, std::deque<Error> &errors);

} // namespace plc_compiler::generator

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_GENERATOR_H