#include "plc_parser.h"

#include "plc_signals.h"

namespace plc_compiler::parser
{

static bool ParseStatement(SharedStatementAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static bool ParseId(SharedIdAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static bool ParseSignal(SharedSignalAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static bool ParseIdOrInputSignal(SharedIdAstNode &id_node, SharedSignalAstNode &signal_node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static bool ParseIdOrOutputSignal(SharedIdAstNode &id_node, SharedSignalAstNode &signal_node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static bool ParseLogicLevel(SharedLogicLevelAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static bool ParseAlias(SharedAliasAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static bool ParseSet(SharedSetAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static bool ParseExpression(SharedExpressionAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static bool ParseElse(SharedElseAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static bool ParseBody(SharedBodyAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static bool ParseIf(SharedIfAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static bool ParseOperator(ExpressionAstNode::Operator &boolean_operator, std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static bool ParseSymbol(std::deque<lexer::Token> &tokens, std::deque<Error> &errors, std::string_view symbol);
static inline bool ParseEquals(std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static inline bool ParseSemicolon(std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static inline bool ParseOpenBrace(std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static inline bool ParseClosedBrace(std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static inline lexer::Token GetToken(std::deque<lexer::Token> &tokens);
static inline void AddUnexpectedEndError(std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static inline bool VerifyRemainingTokens(std::deque<lexer::Token> &tokens, std::deque<Error> &errors, const size_t tokens_required);
static inline bool IsAlias(std::string_view lexeme);
static inline bool IsElseNext(const std::deque<lexer::Token> &tokens);
static inline bool IsBodyNext(const std::deque<lexer::Token> &tokens);
static bool ExpressionToPostfix(std::deque<lexer::Token> &postfix_expression, std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
template<typename T>
static inline void StreamOutNullableArg(std::ostream& stream, const T arg);

StatementAstNode::StatementAstNode(SharedAliasAstNode alias_node) :  node(alias_node)
{
}

StatementAstNode::StatementAstNode(SharedSetAstNode set_node) :  node(set_node)
{
}

StatementAstNode::StatementAstNode(SharedIfAstNode if_node) :  node(if_node)
{
}

IdAstNode::IdAstNode(const std::string &id) : id(id)
{
}

SignalAstNode::SignalAstNode(void) : type(SignalType::SIGNALTYPE_INPUT), signal(0)
{
}

SignalAstNode::SignalAstNode(const wayside_controller::SignalId signal, const SignalType type) : type(type), signal(signal)
{
}

LogicLevelAstNode::LogicLevelAstNode(void) : logic_level(LogicLevel::LOGICLEVEL_LOW)
{
}

LogicLevelAstNode::LogicLevelAstNode(const LogicLevel logic_level) : logic_level(logic_level)
{
}

AliasAstNode::AliasAstNode(void) : id_node(nullptr), signal_node(nullptr)
{
}

AliasAstNode::AliasAstNode(SharedIdAstNode id_node, SharedSignalAstNode signal_node) : id_node(id_node), signal_node(signal_node)
{
}

SetAstNode::SetAstNode(SharedIdAstNode id_node, SharedLogicLevelAstNode logic_level_node) : signal(id_node), logic_level_node(logic_level_node)
{
}

SetAstNode::SetAstNode(SharedSignalAstNode signal_node, SharedLogicLevelAstNode logic_level_node) : signal(signal_node), logic_level_node(logic_level_node)
{
}

ExpressionAstNode::ExpressionAstNode(void) : boolean_operator(Operator::OPERATOR_MAX)
{
}

ExpressionAstNode::ExpressionAstNode(SharedIdAstNode id_node, SharedLogicLevelAstNode logic_level_node) : left_operand(id_node), right_operand(logic_level_node), boolean_operator(Operator::OPERATOR_COMPARISON)
{
}

ExpressionAstNode::ExpressionAstNode(SharedSignalAstNode signal_node, SharedLogicLevelAstNode logic_level_node) : left_operand(signal_node), right_operand(logic_level_node), boolean_operator(Operator::OPERATOR_COMPARISON)
{
}

ExpressionAstNode::ExpressionAstNode(SharedExpressionAstNode left_expression_node, SharedExpressionAstNode right_expression_node, const Operator boolean_operator) : left_operand(left_expression_node), right_operand(right_expression_node), boolean_operator(boolean_operator)
{
}

ElseAstNode::ElseAstNode(SharedIfAstNode if_node) : predicate_node(if_node)
{
}

ElseAstNode::ElseAstNode(SharedBodyAstNode body_node) : predicate_node(body_node)
{
}

BodyAstNode::BodyAstNode(SharedStatementAstNode statement_node) : statement_node(statement_node)
{
}

IfAstNode::IfAstNode(void) : has_else_node(false)
{
}

IfAstNode::IfAstNode(SharedExpressionAstNode expression_node, SharedBodyAstNode body_node) : has_else_node(false), expression_node(expression_node), body_node(body_node), else_node(nullptr)
{
}

IfAstNode::IfAstNode(SharedExpressionAstNode expression_node, SharedBodyAstNode body_node, SharedElseAstNode else_node) : has_else_node(true), expression_node(expression_node), body_node(body_node), else_node(else_node)
{
}

Error::Error(void) : error_type(ErrorType::ERRORTYPE_MAX), token(lexer::TokenType::TOKENTYPE_MAX, "")
{
}

Error::Error(const ErrorType error_type, const lexer::Token &token) : error_type(error_type), token(token)
{
}

bool Error::operator==(const Error &error_right) const
{
    return (error_right.error_type == error_type) && (error_right.token == token);
}

std::ostream& operator<<(std::ostream& stream, const StatementAstNode& node)
{
    stream << "Statement: ";

    std::visit([&stream](auto &&arg)
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, SharedAliasAstNode>)
            {
                stream << "ALIAS";
            }
            else if constexpr (std::is_same_v<T, SharedSetAstNode>)
            {
                stream << "SET";
            }
            else if constexpr (std::is_same_v<T, SharedIfAstNode>)
            {
                stream << "IF";
            }
            else
            {
                stream << "UNKNOWN";
            }

            stream << "\n";
            StreamOutNullableArg<T>(stream, arg);
        }, node.node);

    if (nullptr != node.next_statement)
    {
        stream << "\n";
        stream << *node.next_statement;
    }

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const IdAstNode& node)
{
    stream << "ID: " << node.id;

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const SignalAstNode& node)
{
    stream << "Signal: ";

    switch (node.type)
    {
    case SignalAstNode::SignalType::SIGNALTYPE_INPUT:
        stream << "INPUT";
        break;
    case SignalAstNode::SignalType::SIGNALTYPE_OUTPUT:
        stream << "OUTPUT";
        break;
    default:
        stream << "UNKNOWN";
        break;
    }

    stream << " " << node.signal;

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const LogicLevelAstNode& node)
{
    stream << "Logic level: ";

    switch (node.logic_level)
    {
    case LogicLevelAstNode::LogicLevel::LOGICLEVEL_LOW:
        stream << "LOW";
        break;
    case LogicLevelAstNode::LogicLevel::LOGICLEVEL_HIGH:
        stream << "HIGH";
        break;
    default:
        stream << "UNKNOWN";
        break;
    }

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const AliasAstNode& node)
{
    stream << "Alias:\n";
    StreamOutNullableArg<SharedIdAstNode>(stream, node.id_node);
    stream << "\n";
    StreamOutNullableArg<SharedSignalAstNode>(stream, node.signal_node);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const SetAstNode& node)
{
    stream << "Set: ";

    std::visit([&stream](auto &&arg)
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, SharedIdAstNode>)
            {
                stream << "ID";
            }
            else if constexpr (std::is_same_v<T, SharedSignalAstNode>)
            {
                stream << "SIGNAL";
            }
            else
            {
                stream << "UNKNOWN";
            }

            stream << "\n";
            StreamOutNullableArg<T>(stream, arg);
        }, node.signal);

    stream << "\n";
    StreamOutNullableArg<SharedLogicLevelAstNode>(stream, node.logic_level_node);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const ExpressionAstNode& node)
{
    stream << "Expression: ";

    std::visit([&stream](auto &&arg)
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, SharedIdAstNode>)
            {
                stream << "ID";
            }
            else if constexpr (std::is_same_v<T, SharedSignalAstNode>)
            {
                stream << "SIGNAL";
            }
            else if constexpr (std::is_same_v<T, SharedExpressionAstNode>)
            {
                stream << "EXPRESSION";
            }
            else
            {
                stream << "UNKNOWN";
            }

            stream << "\nLeft operand: ";
            StreamOutNullableArg<T>(stream, arg);
        }, node.left_operand);

    stream << "\nOperator: ";

    switch (node.boolean_operator)
    {
    case ExpressionAstNode::Operator::OPERATOR_COMPARISON:
        stream << "COMPARISON";
        break;
    case ExpressionAstNode::Operator::OPERATOR_AND:
        stream << "AND";
        break;
    case ExpressionAstNode::Operator::OPERATOR_OR:
        stream << "OR";
        break;
    default:
        stream << "UNKNOWN";
        break;
    }

    stream << "\nRight operand: ";

    std::visit([&stream](auto &&arg)
        {
            using T = std::decay_t<decltype(arg)>;
            StreamOutNullableArg<T>(stream, arg);
        }, node.right_operand);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const ElseAstNode& node)
{
    stream << "Else: ";

    std::visit([&stream](auto &&arg)
        {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, SharedIfAstNode>)
            {
                stream << "IF";
            }
            else if constexpr (std::is_same_v<T, SharedBodyAstNode>)
            {
                stream << "BODY";
            }
            else
            {
                stream << "UNKNOWN";
            }

            stream << "\n";
            StreamOutNullableArg(stream, arg);
        }, node.predicate_node);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const BodyAstNode& node)
{
    stream << "Body:\n";
    StreamOutNullableArg<SharedStatementAstNode>(stream, node.statement_node);

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const IfAstNode& node)
{
    stream << "If:\n";
    StreamOutNullableArg(stream, node.expression_node);
    stream << "\n";
    StreamOutNullableArg(stream, node.body_node);

    if (node.has_else_node)
    {
        stream << "\n";
        StreamOutNullableArg(stream, node.else_node);
    }

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Error& error)
{
    using enum ErrorType;

    stream << "Syntax error: ";

    switch (error.error_type)
    {
    case ERRORTYPE_INVALID_INPUT:
        stream << "Invalid input";
        break;
    case ERRORTYPE_KEYWORD_AS_ID:
        stream << "Keyword cannot be used as an identifier";
        break;
    case ERRORTYPE_INVALID_SIGNAL:
        stream << "Invalid signal";
        break;
    case ERRORTYPE_INVALID_LOGIC_LEVEL:
        stream << "Invalid logic level";
        break;
    case ERRORTYPE_MISSING_SYMBOL:
        stream << "Missing symbol";
        break;
    case ERRORTYPE_ILLEGAL_SYMBOL:
        stream << "Illegal symbol";
        break;
    case ERRORTYPE_UNEXPECTED_END:
        stream << "Unexpected end of file";
        break;
    default:
        stream << "Unknown error";
        break;
    }

    stream << " \"" << error.token.lexeme << "\"";

    return stream;
}

SharedStatementAstNode Parse(std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    SharedStatementAstNode root = nullptr;
    SharedStatementAstNode next = nullptr;

    errors.clear();

    while (!tokens.empty())
    {
        SharedStatementAstNode new_node = nullptr;

        if (!ParseStatement(new_node, tokens, errors))
        {
            // Parse function sets error, do nothing
        }
        else if (nullptr == root)
        {
            root = new_node;
            next = new_node;
        }
        else
        {
            next->next_statement = new_node;
            next                 = new_node;
        }
    }

    return root;
}


static bool ParseStatement(SharedStatementAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    bool               parsed     = false;
    SharedAliasAstNode alias_node = nullptr;
    SharedSetAstNode   set_node   = nullptr;
    SharedIfAstNode    if_node    = nullptr;
    node = nullptr;

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else
    {
        size_t       tokens_size = tokens.size();
        lexer::Token token       = tokens.front();

        if (IsAlias(token.lexeme) && ParseAlias(alias_node, tokens, errors))
        {
            node   = std::make_shared<StatementAstNode>(alias_node);
            parsed = true;
        }
        else if (("SET" == token.lexeme) && ParseSet(set_node, tokens, errors))
        {
            node   = std::make_shared<StatementAstNode>(set_node);
            parsed = true;
        }
        else if (("IF" == token.lexeme) && ParseIf(if_node, tokens, errors))
        {
            node   = std::make_shared<StatementAstNode>(if_node);
            parsed = true;
        }
        else
        {
            errors.emplace_back(ErrorType::ERRORTYPE_INVALID_INPUT, token);

            if (tokens.size() == tokens_size)
            {
                tokens.pop_front();
            }
        }
    }

    return parsed;
}

static bool ParseId(SharedIdAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    bool parsed = false;
    node = nullptr;

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else
    {
        lexer::Token token = GetToken(tokens);

        if (lexer::TokenType::TOKENTYPE_KEYWORD == token.token_type)
        {
            errors.emplace_back(ErrorType::ERRORTYPE_KEYWORD_AS_ID, token);
        }
        else if (lexer::TokenType::TOKENTYPE_ID != token.token_type)
        {
            errors.emplace_back(ErrorType::ERRORTYPE_INVALID_INPUT, token);
        }
        else
        {
            node   = std::make_shared<IdAstNode>(token.lexeme);
            parsed = true;
        }
    }

    return parsed;
}

static bool ParseSignal(SharedSignalAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    bool parsed = false;
    node = nullptr;

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else
    {
        wayside_controller::SignalId signal = 0;
        lexer::Token                 token  = GetToken(tokens);

        if ((lexer::TokenType::TOKENTYPE_INPUT_SIGNAL == token.token_type) && GetInput(token.lexeme, signal))
        {
            node   = std::make_shared<SignalAstNode>(signal, SignalAstNode::SignalType::SIGNALTYPE_INPUT);
            parsed = true;
        }
        else if ((lexer::TokenType::TOKENTYPE_OUTPUT_SIGNAL == token.token_type) && GetOutput(token.lexeme, signal))
        {
            node   = std::make_shared<SignalAstNode>(signal, SignalAstNode::SignalType::SIGNALTYPE_OUTPUT);
            parsed = true;
        }
        else
        {
            errors.emplace_back(ErrorType::ERRORTYPE_INVALID_SIGNAL, token);
        }
    }

    return parsed;
}

static bool ParseIdOrInputSignal(SharedIdAstNode &id_node, SharedSignalAstNode &signal_node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    bool parsed = false;
    id_node     = nullptr;
    signal_node = nullptr;

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else
    {
        lexer::Token token = tokens.front();

        if (lexer::TokenType::TOKENTYPE_ID == token.token_type)
        {
            parsed = ParseId(id_node, tokens, errors);
        }
        else if (lexer::TokenType::TOKENTYPE_INPUT_SIGNAL == token.token_type)
        {
            parsed = ParseSignal(signal_node, tokens, errors);
        }
        else if (lexer::TokenType::TOKENTYPE_OUTPUT_SIGNAL == token.token_type)
        {
            errors.emplace_back(ErrorType::ERRORTYPE_INVALID_SIGNAL, token);
            tokens.pop_front();
        }
        else
        {
            errors.emplace_back(ErrorType::ERRORTYPE_INVALID_INPUT, token);
            tokens.pop_front();
        }
    }

    return parsed;
}

static bool ParseIdOrOutputSignal(SharedIdAstNode &id_node, SharedSignalAstNode &signal_node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    bool parsed = false;
    id_node     = nullptr;
    signal_node = nullptr;

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else
    {
        lexer::Token token = tokens.front();

        if (lexer::TokenType::TOKENTYPE_ID == token.token_type)
        {
            parsed = ParseId(id_node, tokens, errors);
        }
        else if (lexer::TokenType::TOKENTYPE_OUTPUT_SIGNAL == token.token_type)
        {
            parsed = ParseSignal(signal_node, tokens, errors);
        }
        else if (lexer::TokenType::TOKENTYPE_INPUT_SIGNAL == token.token_type)
        {
            errors.emplace_back(ErrorType::ERRORTYPE_INVALID_SIGNAL, token);
            tokens.pop_front();
        }
        else
        {
            errors.emplace_back(ErrorType::ERRORTYPE_INVALID_INPUT, token);
            tokens.pop_front();
        }
    }

    return parsed;
}

static bool ParseLogicLevel(SharedLogicLevelAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    bool parsed = false;
    node = nullptr;

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else
    {
        lexer::Token token = GetToken(tokens);

        if ("LOW" == token.lexeme)
        {
            node   = std::make_shared<LogicLevelAstNode>(LogicLevelAstNode::LogicLevel::LOGICLEVEL_LOW);
            parsed = true;
        }
        else if ("HIGH" == token.lexeme)
        {
            node   = std::make_shared<LogicLevelAstNode>(LogicLevelAstNode::LogicLevel::LOGICLEVEL_HIGH);
            parsed = true;
        }
        else
        {
            errors.emplace_back(ErrorType::ERRORTYPE_INVALID_LOGIC_LEVEL, token);
        }

    }

    return parsed;
}

static bool ParseAlias(SharedAliasAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    bool parsed = false;
    node = nullptr;

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else
    {
        SharedIdAstNode     id_node     = nullptr;
        SharedSignalAstNode signal_node = nullptr;
        lexer::Token        alias_token = GetToken(tokens);

        if (!IsAlias(alias_token.lexeme))
        {
            errors.emplace_back(ErrorType::ERRORTYPE_INVALID_INPUT, alias_token);
        }
        else if (!ParseId(id_node, tokens, errors))
        {
            // Parse function sets error, do nothing
        }
        else if (!ParseEquals(tokens, errors))
        {
            // Parse function sets error, do nothing
        }
        else if (!ParseSignal(signal_node, tokens, errors))
        {
            // Parse function sets error, do nothing
        }
        else if (!ParseSemicolon(tokens, errors))
        {
            // Parse function sets error, do nothing
        }
        else if (("BLOCK" == alias_token.lexeme) && (SignalAstNode::SignalType::SIGNALTYPE_INPUT != signal_node->type))
        {
            errors.emplace_back(ErrorType::ERRORTYPE_INVALID_SIGNAL, lexer::Token(lexer::TokenType::TOKENTYPE_OUTPUT_SIGNAL, std::to_string(signal_node->signal)));
        }
        else if (("BLOCK" != alias_token.lexeme) && (SignalAstNode::SignalType::SIGNALTYPE_INPUT == signal_node->type))
        {
            errors.emplace_back(ErrorType::ERRORTYPE_INVALID_SIGNAL, lexer::Token(lexer::TokenType::TOKENTYPE_INPUT_SIGNAL, std::to_string(signal_node->signal)));
        }
        else
        {
            node   = std::make_shared<AliasAstNode>(id_node, signal_node);
            parsed = true;
        }
    }

    return parsed;
}

static bool ParseSet(SharedSetAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    bool                    parsed           = false;
    SharedIdAstNode         id_node          = nullptr;
    SharedSignalAstNode     signal_node      = nullptr;
    SharedLogicLevelAstNode logic_level_node = nullptr;
    node = nullptr;

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else if (lexer::Token set_token = GetToken(tokens); "SET" != set_token.lexeme)
    {
        errors.emplace_back(ErrorType::ERRORTYPE_INVALID_INPUT, set_token);
    }
    else if (!ParseIdOrOutputSignal(id_node, signal_node, tokens, errors))
    {
        // Parse function sets error, do nothing
    }
    else if (!ParseLogicLevel(logic_level_node, tokens, errors))
    {
        // Parse function sets error, do nothing
    }
    else if (!ParseSemicolon(tokens, errors))
    {
        // Parse function sets error, do nothing
    }
    else if (nullptr != id_node)
    {
        node   = std::make_shared<SetAstNode>(id_node, logic_level_node);
        parsed = true;
    }
    else
    {
        node   = std::make_shared<SetAstNode>(signal_node, logic_level_node);
        parsed = true;
    }

    return parsed;
}

static bool ParseExpression(SharedExpressionAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    std::deque<lexer::Token>            postfix_expression;
    bool                                parsed = ExpressionToPostfix(postfix_expression, tokens, errors);
    std::deque<lexer::Token>            operands;
    std::deque<SharedExpressionAstNode> expressions;
    node = nullptr;

    while (!postfix_expression.empty() && parsed)
    {
        ExpressionAstNode::Operator boolean_operator = ExpressionAstNode::Operator::OPERATOR_MAX;
        lexer::Token                token            = GetToken(postfix_expression);

        if (lexer::TokenType::TOKENTYPE_SYMBOL != token.token_type)
        {
            operands.emplace_back(token);
        }
        else if (operands.size() > 1)
        {
            SharedIdAstNode          id_node          = nullptr;
            SharedSignalAstNode      signal_node      = nullptr;
            SharedLogicLevelAstNode  logic_level_node = nullptr;
            std::deque<lexer::Token> expression_tokens;

            expression_tokens.emplace_front(operands.back());
            operands.pop_back();
            expression_tokens.emplace_front(token);
            expression_tokens.emplace_front(operands.back());
            operands.pop_back();

            if (!ParseIdOrInputSignal(id_node, signal_node, expression_tokens, errors) ||
                !ParseOperator(boolean_operator, expression_tokens, errors) ||
                !ParseLogicLevel(logic_level_node, expression_tokens, errors))
            {
                parsed = false;
            }
            else if (nullptr != id_node)
            {
                expressions.push_back(std::make_shared<ExpressionAstNode>(id_node, logic_level_node));
            }
            else
            {
                expressions.push_back(std::make_shared<ExpressionAstNode>(signal_node, logic_level_node));
            }
        }
        else if (expressions.size() > 1)
        {
            std::deque<lexer::Token> expression_tokens;
            expression_tokens.emplace_back(token);

            if (!ParseOperator(boolean_operator, expression_tokens, errors))
            {
                parsed = false;
            }
            else
            {
                SharedExpressionAstNode right_expression_node = expressions.back();
                expressions.pop_back();

                SharedExpressionAstNode left_expression_node = expressions.back();
                expressions.pop_back();

                expressions.push_back(std::make_shared<ExpressionAstNode>(left_expression_node, right_expression_node, boolean_operator));
            }
        }
        else
        {
            errors.emplace_back(ErrorType::ERRORTYPE_ILLEGAL_SYMBOL, token);
            parsed = false;
        }
    }

    if (!parsed)
    {
        // Not parsed, nothing to check
    }
    else if (!operands.empty())
    {
        errors.emplace_back(ErrorType::ERRORTYPE_MISSING_SYMBOL, operands.back());
        parsed = false;
    }
    else if (expressions.size() > 1)
    {
        errors.emplace_back(ErrorType::ERRORTYPE_MISSING_SYMBOL, lexer::Token(lexer::TokenType::TOKENTYPE_SYMBOL, "("));
        parsed = false;
    }
    else
    {
        node = expressions.front();
    }

    return parsed;
}

static bool ParseElse(SharedElseAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    bool parsed = false;
    node = nullptr;

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else if (lexer::Token token = GetToken(tokens); "ELSE" != token.lexeme)
    {
        errors.emplace_back(ErrorType::ERRORTYPE_INVALID_INPUT, token);
    }
    else if (IsBodyNext(tokens))
    {
        SharedBodyAstNode body_node = nullptr;

        if (!ParseBody(body_node, tokens, errors))
        {
            // Parse function sets error, do nothing
        }
        else
        {
            node   = std::make_shared<ElseAstNode>(body_node);
            parsed = true;
        }
    }
    else
    {
        SharedIfAstNode if_node = nullptr;

        if (!ParseIf(if_node, tokens, errors))
        {
            // Parse function sets error, do nothing
        }
        else
        {
            node   = std::make_shared<ElseAstNode>(if_node);
            parsed = true;
        }
    }

    return parsed;
}

static bool ParseBody(SharedBodyAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    bool                   parsed         = ParseOpenBrace(tokens, errors);
    SharedStatementAstNode statement_node = nullptr;
    SharedStatementAstNode next           = nullptr;
    node = nullptr;

    while (VerifyRemainingTokens(tokens, errors, 1) && parsed)
    {
        SharedStatementAstNode new_node = nullptr;

        if ("}" == tokens.front().lexeme)
        {
            break;
        }
        else if (!ParseStatement(new_node, tokens, errors))
        {
            parsed = false;
        }
        else if (nullptr == statement_node)
        {
            statement_node = new_node;
            next           = new_node;
        }
        else
        {
            next->next_statement = new_node;
            next                 = new_node;
        }
    }

    if (parsed)
    {
        if (!ParseClosedBrace(tokens, errors))
        {
            parsed = false;
        }
        else
        {
            node = std::make_shared<BodyAstNode>(statement_node);
        }
    }

    return parsed;
}

static bool ParseIf(SharedIfAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    bool parsed = false;
    node = nullptr;

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else
    {
        SharedExpressionAstNode expression_node = nullptr;
        SharedBodyAstNode       body_node       = nullptr;
        SharedElseAstNode       else_node       = nullptr;

        if (lexer::Token token = GetToken(tokens); "IF" != token.lexeme)
        {
            errors.emplace_back(ErrorType::ERRORTYPE_INVALID_INPUT, token);
        }
        else if (!ParseExpression(expression_node, tokens, errors))
        {
            // Parse function sets error, do nothing
        }
        else if (!ParseBody(body_node, tokens, errors))
        {
            // Parse function sets error, do nothing
        }
        else if (IsElseNext(tokens) && !ParseElse(else_node, tokens, errors))
        {
            // Parse function sets error, do nothing
        }
        else if (nullptr != else_node)
        {
            node   = std::make_shared<IfAstNode>(expression_node, body_node, else_node);
            parsed = true;
        }
        else
        {
            node   = std::make_shared<IfAstNode>(expression_node, body_node);
            parsed = true;
        }
    }

    return parsed;
}

static bool ParseOperator(ExpressionAstNode::Operator &boolean_operator, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    bool is_operator = false;
    boolean_operator = ExpressionAstNode::Operator::OPERATOR_MAX;

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else
    {
        lexer::Token token = GetToken(tokens);

        if ("==" == token.lexeme)
        {
            boolean_operator = ExpressionAstNode::Operator::OPERATOR_COMPARISON;
            is_operator      = true;
        }
        else if ("&&" == token.lexeme)
        {
            boolean_operator = ExpressionAstNode::Operator::OPERATOR_AND;
            is_operator      = true;
        }
        else if ("||" == token.lexeme)
        {
            boolean_operator = ExpressionAstNode::Operator::OPERATOR_OR;
            is_operator      = true;
        }
        else if (lexer::TokenType::TOKENTYPE_SYMBOL == token.token_type)
        {
            errors.emplace_back(ErrorType::ERRORTYPE_ILLEGAL_SYMBOL, token);
        }
        else
        {
            errors.emplace_back(ErrorType::ERRORTYPE_MISSING_SYMBOL, token);
        }
    }

    return is_operator;
}

static bool ParseSymbol(std::deque<lexer::Token> &tokens, std::deque<Error> &errors, std::string_view symbol)
{
    bool is_symbol = false;

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else if (lexer::Token token = GetToken(tokens); symbol != token.lexeme)
    {
        if (lexer::TokenType::TOKENTYPE_SYMBOL != token.token_type)
        {
            errors.emplace_back(ErrorType::ERRORTYPE_MISSING_SYMBOL, token);
        }
        else
        {
            errors.emplace_back(ErrorType::ERRORTYPE_ILLEGAL_SYMBOL, token);
        }
    }
    else
    {
        is_symbol = true;
    }

    return is_symbol;
}

static inline bool ParseEquals(std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    return ParseSymbol(tokens, errors, "=");
}

static inline bool ParseSemicolon(std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    return ParseSymbol(tokens, errors, ";");
}

static inline bool ParseOpenBrace(std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    return ParseSymbol(tokens, errors, "{");
}

static inline bool ParseClosedBrace(std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    return ParseSymbol(tokens, errors, "}");
}

static inline lexer::Token GetToken(std::deque<lexer::Token> &tokens)
{
    lexer::Token token = tokens.front();

    tokens.pop_front();

    return token;
}

static inline void AddUnexpectedEndError(std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    lexer::Token token;

    if (!tokens.empty())
    {
        token = tokens.back();
    }

    errors.emplace_back(ErrorType::ERRORTYPE_UNEXPECTED_END, token);
    tokens.clear();
}

static inline bool VerifyRemainingTokens(std::deque<lexer::Token> &tokens, std::deque<Error> &errors, const size_t tokens_required)
{
    bool remaining_tokens = false;

    if (tokens.size() < tokens_required)
    {
        AddUnexpectedEndError(tokens, errors);
    }
    else
    {
        remaining_tokens = true;
    }

    return remaining_tokens;
}

static inline bool IsAlias(std::string_view lexeme)
{
    bool is_alias = false;

    if (("BLOCK" == lexeme) || ("SWITCH" == lexeme) || ("LIGHT" == lexeme) || ("CROSSING" == lexeme))
    {
        is_alias = true;
    }

    return is_alias;
}

static inline bool IsElseNext(const std::deque<lexer::Token> &tokens)
{
    bool is_else_next = false;

    if (!tokens.empty() && ("ELSE" == tokens.front().lexeme))
    {
        is_else_next = true;
    }

    return is_else_next;
}

static inline bool IsBodyNext(const std::deque<lexer::Token> &tokens)
{
    bool is_body_next = false;

    if (!tokens.empty() && ("{" == tokens.front().lexeme))
    {
        is_body_next = true;
    }

    return is_body_next;
}

static bool ExpressionToPostfix(std::deque<lexer::Token> &postfix_expression, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    bool                     converted   = false;
    bool                     symbol_next = false;
    std::deque<lexer::Token> symbols;

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else if ("(" != tokens.front().lexeme)
    {
        lexer::Token token = GetToken(tokens);

        if (lexer::TokenType::TOKENTYPE_SYMBOL == token.token_type)
        {
            errors.emplace_back(ErrorType::ERRORTYPE_ILLEGAL_SYMBOL, token);
        }
        else
        {
            errors.emplace_back(ErrorType::ERRORTYPE_MISSING_SYMBOL, token);
        }
    }
    else
    {
        symbols.emplace_back(GetToken(tokens));
        converted = true;
    }

    while (!symbols.empty() && !tokens.empty())
    {
        lexer::Token token = GetToken(tokens);

        if (")" == token.lexeme)
        {
            while ("(" != symbols.back().lexeme)
            {
                postfix_expression.emplace_back(symbols.back());
                symbols.pop_back();
            }

            symbols.pop_back();
            symbol_next = false;
        }
        else if (lexer::TokenType::TOKENTYPE_SYMBOL == token.token_type)
        {
            symbols.emplace_back(token);
            symbol_next = false;
        }
        else if (symbol_next)
        {
            errors.emplace_back(ErrorType::ERRORTYPE_MISSING_SYMBOL, token);
            converted = false;

            break;
        }
        else
        {
            postfix_expression.emplace_back(token);
            symbol_next = true;
        }
    }

    if (converted && !symbols.empty())
    {
        if (!postfix_expression.empty())
        {
            errors.emplace_back(ErrorType::ERRORTYPE_MISSING_SYMBOL, postfix_expression.back());
        }
        else
        {
            errors.emplace_back(ErrorType::ERRORTYPE_MISSING_SYMBOL, symbols.back());
        }

        converted = false;
    }

    return converted;
}

template<typename T>
static inline void StreamOutNullableArg(std::ostream& stream, const T arg)
{
    if (nullptr == arg)
    {
        stream << "nullptr";
    }
    else
    {
        stream << *arg;
    }
}

} // namespace plc_compiler::parser