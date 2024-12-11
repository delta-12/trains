/*****************************************************************************
* @file plc_generator.h
*
* @brief Implements the code generator for the PLC program compiler.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_GENERATOR_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_GENERATOR_H

#include <deque>
#include <variant>

#include "plc_parser.h"

namespace plc_compiler::generator
{

using Error = struct Error;

struct Error
{
    public:
        Error(parser::SharedIdAstNode node);
        Error(parser::SharedExpressionAstNode node);
        friend std::ostream& operator<<(std::ostream& stream, const Error& error);
        std::variant<parser::SharedIdAstNode, parser::SharedExpressionAstNode> node;
};

bool GenerateCode(parser::SharedStatementAstNode root, void *const buffer, const size_t size, size_t &bytes_written, std::deque<Error> &errors);

} // namespace plc_compiler::generator

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_GENERATOR_H