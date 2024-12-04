/*****************************************************************************
* @file plc_parser.h
*
* @brief Implements the parser for the PLC program compiler.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_PARSER_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_PARSER_H

#include <deque>
#include <memory>
#include <variant>

#include "plc_lexer.h"
#include "wayside_controller.h"

namespace plc_compiler::parser
{

using StatementAstNode  = struct StatementAstNode;
using IdAstNode         = struct IdAstNode;
using SignalAstNode     = struct SignalAstNode;
using LogicLevelAstNode = struct LogicLevelAstNode;
using AliasAstNode      = struct AliasAstNode;
using SetAstNode        = struct SetAstNode;
using ExpressionAstNode = struct ExpressionAstNode;
using ElseAstNode       = struct ElseAstNode;
using BodyAstNode       = struct BodyAstNode;
using IfAstNode         = struct IfAstNode;

using SharedStatementAstNode  = std::shared_ptr<StatementAstNode>;
using SharedIdAstNode         = std::shared_ptr<IdAstNode>;
using SharedSignalAstNode     = std::shared_ptr<SignalAstNode>;
using SharedLogicLevelAstNode = std::shared_ptr<LogicLevelAstNode>;
using SharedAliasAstNode      = std::shared_ptr<AliasAstNode>;
using SharedSetAstNode        = std::shared_ptr<SetAstNode>;
using SharedExpressionAstNode = std::shared_ptr<ExpressionAstNode>;
using SharedElseAstNode       = std::shared_ptr<ElseAstNode>;
using SharedBodyAstNode       = std::shared_ptr<BodyAstNode>;
using SharedIfAstNode         = std::shared_ptr<IfAstNode>;

enum class ErrorType
{
    ERRORTYPE_INVALID_INPUT,
    ERRORTYPE_KEYWORD_AS_ID,
    ERRORTYPE_INVALID_SIGNAL,
    ERRORTYPE_INVALID_LOGIC_LEVEL,
    ERRORTYPE_MISSING_SYMBOL,
    ERRORTYPE_ILLEGAL_SYMBOL,
    ERRORTYPE_UNEXPECTED_END,
    ERRORTYPE_MAX
};

struct StatementAstNode
{
    public:
        explicit StatementAstNode(SharedAliasAstNode alias_node);
        explicit StatementAstNode(SharedSetAstNode set_node);
        explicit StatementAstNode(SharedIfAstNode if_node);
        friend std::ostream& operator<<(std::ostream& stream, const StatementAstNode& node);
        std::variant<SharedAliasAstNode, SharedSetAstNode, SharedIfAstNode> node;
        SharedStatementAstNode next_statement = nullptr;
};

struct IdAstNode
{
    public:
        explicit IdAstNode(const std::string &id);
        friend std::ostream& operator<<(std::ostream& stream, const IdAstNode& node);
        std::string id;
};

struct SignalAstNode
{
    public:
        enum class SignalType
        {
            SIGNALTYPE_INPUT,
            SIGNALTYPE_OUTPUT
        };

        SignalAstNode(void);
        SignalAstNode(const wayside_controller::SignalId signal, const SignalType type);
        friend std::ostream& operator<<(std::ostream& stream, const SignalAstNode& node);
        SignalType type;
        wayside_controller::SignalId signal;
};

struct LogicLevelAstNode
{
    public:
        enum class LogicLevel
        {
            LOGICLEVEL_LOW,
            LOGICLEVEL_HIGH
        };

        LogicLevelAstNode(void);
        explicit LogicLevelAstNode(const LogicLevel logic_level);
        friend std::ostream& operator<<(std::ostream& stream, const LogicLevelAstNode& node);
        LogicLevel logic_level;
};

struct AliasAstNode
{
    public:
        AliasAstNode(void);
        AliasAstNode(SharedIdAstNode id_node, SharedSignalAstNode signal_node);
        friend std::ostream& operator<<(std::ostream& stream, const AliasAstNode& node);
        SharedIdAstNode id_node;
        SharedSignalAstNode signal_node;
};

struct SetAstNode
{
    public:
        SetAstNode(SharedIdAstNode id_node, SharedLogicLevelAstNode logic_level_node);
        SetAstNode(SharedSignalAstNode signal_node, SharedLogicLevelAstNode logic_level_node);
        friend std::ostream& operator<<(std::ostream& stream, const SetAstNode& node);
        std::variant<SharedIdAstNode, SharedSignalAstNode> signal;
        SharedLogicLevelAstNode logic_level_node;
};

struct ExpressionAstNode
{
    public:
        enum class Operator
        {
            OPERATOR_COMPARISON,
            OPERATOR_AND,
            OPERATOR_OR,
            OPERATOR_MAX
        };

        ExpressionAstNode(void);
        ExpressionAstNode(SharedIdAstNode id_node, SharedLogicLevelAstNode logic_level_node);
        ExpressionAstNode(SharedSignalAstNode signal_node, SharedLogicLevelAstNode logic_level_node);
        ExpressionAstNode(SharedExpressionAstNode left_expression_node, SharedExpressionAstNode right_expression_node, const Operator boolean_operator);
        friend std::ostream& operator<<(std::ostream& stream, const ExpressionAstNode& node);
        std::variant<SharedIdAstNode, SharedSignalAstNode, SharedExpressionAstNode> left_operand;
        std::variant<SharedLogicLevelAstNode, SharedExpressionAstNode> right_operand;
        Operator boolean_operator;
};

struct ElseAstNode
{
    public:
        explicit ElseAstNode(SharedIfAstNode if_node);
        explicit ElseAstNode(SharedBodyAstNode body_node);
        friend std::ostream& operator<<(std::ostream& stream, const ElseAstNode& node);
        std::variant<SharedIfAstNode, SharedBodyAstNode> predicate_node;
};

struct BodyAstNode
{
    public:
        explicit BodyAstNode(SharedStatementAstNode statement_node);
        friend std::ostream& operator<<(std::ostream& stream, const BodyAstNode& node);
        SharedStatementAstNode statement_node;
};


struct IfAstNode
{
    public:
        IfAstNode(void);
        IfAstNode(SharedExpressionAstNode expression_node, SharedBodyAstNode body_node);
        IfAstNode(SharedExpressionAstNode expression_node, SharedBodyAstNode body_node, SharedElseAstNode else_node);
        friend std::ostream& operator<<(std::ostream& stream, const IfAstNode& node);
        bool has_else_node;
        SharedExpressionAstNode expression_node;
        SharedBodyAstNode body_node;
        SharedElseAstNode else_node;
};

struct Error
{
    public:
        Error(void);
        Error(const ErrorType error_type, const lexer::Token &token);
        bool operator==(const Error &error_right) const;
        friend std::ostream& operator<<(std::ostream& stream, const Error& error);
        ErrorType error_type;
        lexer::Token token;
};

SharedStatementAstNode Parse(std::deque<lexer::Token> &tokens, std::deque<Error> &errors);

} // namespace plc_compiler::parser

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_PARSER_H