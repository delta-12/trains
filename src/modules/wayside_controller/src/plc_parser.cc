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
static bool ParseSymbol(std::deque<lexer::Token> &tokens, std::deque<Error> &errors, const std::string &symbol);
static inline bool ParseEquals(std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static inline bool ParseSemicolon(std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static inline bool ParseOpenParenthesis(std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static inline bool ParseClosedParenthesis(std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static inline bool ParseOpenBracket(std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static inline bool ParseClosedBracket(std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static inline bool IsAlias(const std::string &lexeme);
static inline lexer::Token GetToken(std::deque<lexer::Token> &tokens);
static inline void AddUnexpectedEndError(std::deque<lexer::Token> &tokens, std::deque<Error> &errors);
static inline bool VerifyRemainingTokens(std::deque<lexer::Token> &tokens, std::deque<Error> &errors, const size_t tokens_required);
static inline bool IsElseNext(std::deque<lexer::Token> &tokens);

StatementAstNode::StatementAstNode(void)
{
}

StatementAstNode::StatementAstNode(SharedAliasAstNode alias_node) :  node(alias_node)
{
}

StatementAstNode::StatementAstNode(SharedSetAstNode set_node) :  node(set_node)
{
}

StatementAstNode::StatementAstNode(SharedIfAstNode if_node) :  node(if_node)
{
}

IdAstNode::IdAstNode(void)
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

SetAstNode::SetAstNode(void)
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

ExpressionAstNode::ExpressionAstNode(SharedExpressionAstNode left_expression_node, SharedExpressionAstNode right_expression_node, const std::string &operator_lexeme) : left_operand(left_expression_node), right_operand(right_expression_node)
{
    if ("&&" == operator_lexeme)
    {
        boolean_operator = Operator::OPERATOR_AND;
    }
    else if ("||" == operator_lexeme)
    {
        boolean_operator = Operator::OPERATOR_OR;
    }
    else if ("==" == operator_lexeme)
    {
        boolean_operator = Operator::OPERATOR_COMPARISON;
    }
    else
    {
        boolean_operator = Operator::OPERATOR_MAX;
    }
}

ElseAstNode::ElseAstNode(void)
{
}

ElseAstNode::ElseAstNode(SharedIfAstNode if_node) : predicate_node(if_node)
{
}

ElseAstNode::ElseAstNode(SharedBodyAstNode body_node) : predicate_node(body_node)
{
}

BodyAstNode::BodyAstNode(void)
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

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else
    {
        lexer::Token token = tokens.front();

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
        }
    }

    return parsed;
}

static bool ParseId(SharedIdAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    bool parsed = false;

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
            node = std::make_shared<SignalAstNode>(signal, SignalAstNode::SignalType::SIGNALTYPE_INPUT);
        }
        else if ((lexer::TokenType::TOKENTYPE_OUTPUT_SIGNAL == token.token_type) && GetOutput(token.lexeme, signal))
        {
            node = std::make_shared<SignalAstNode>(signal, SignalAstNode::SignalType::SIGNALTYPE_OUTPUT);
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
        else if (SignalAstNode::SignalType::SIGNALTYPE_INPUT == signal_node->type)
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
    bool parsed = false;

    if (!VerifyRemainingTokens(tokens, errors, 1))
    {
        // Not enough tokens remaining, verify function sets error, do nothing
    }
    else if (lexer::Token set_token = GetToken(tokens); "SET" != set_token.lexeme)
    {
        errors.emplace_back(ErrorType::ERRORTYPE_INVALID_INPUT, set_token);
    }
    else
    {
        SharedIdAstNode         id_node          = nullptr;
        SharedSignalAstNode     signal_node      = nullptr;
        SharedLogicLevelAstNode logic_level_node = nullptr;

        if (!ParseIdOrOutputSignal(id_node, signal_node, tokens, errors))
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
            node   = std::make_shared<SetAstNode>(signal_node, logic_level_node);
            parsed = true;
        }
        else
        {
            node   = std::make_shared<SetAstNode>(signal_node, logic_level_node);
            parsed = true;
        }
    }

    return parsed;
}

static bool ParseExpression(SharedExpressionAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    // TODO create new ast node for operand?

    ExpressionAstNode::Operator boolean_operator = ExpressionAstNode::Operator::OPERATOR_MAX;

    if (!ParseOpenParenthesis(tokens, errors))
    {
        // Parse function sets error, do nothing
    }
    // TODO parse left operand
    else if (!ParseOperator(boolean_operator, tokens, errors))
    {
        // Parse function sets error, do nothing
    }
    // TODO parse right operand
    else if (!ParseClosedParenthesis(tokens, errors))
    {
        // Parse function sets error, do nothing
    }
    else
    {
        // TODO new expression node
    }

    return false;
}

static bool ParseElse(SharedElseAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    // TODO
    (void)node;
    (void)tokens;
    (void)errors;

    // Parse else
    // Parse body or parse if

    return false;
}

static bool ParseBody(SharedBodyAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    // TODO
    (void)node;
    (void)tokens;
    (void)errors;

    // Parse {
    // Parse statement
    // Parse }

    return false;
}

static bool ParseIf(SharedIfAstNode &node, std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    (void)node;
    (void)tokens;
    (void)errors;

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
            node = std::make_shared<IfAstNode>(expression_node, body_node, else_node);
        }
        else
        {
            node = std::make_shared<IfAstNode>(expression_node, body_node);
        }
    }

    return false;
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

static bool ParseSymbol(std::deque<lexer::Token> &tokens, std::deque<Error> &errors, const std::string &symbol)
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

static inline bool ParseOpenParenthesis(std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    return ParseSymbol(tokens, errors, "(");
}

static inline bool ParseClosedParenthesis(std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    return ParseSymbol(tokens, errors, ")");
}

static inline bool ParseOpenBracket(std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    return ParseSymbol(tokens, errors, "{");
}

static inline bool ParseClosedBracket(std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    return ParseSymbol(tokens, errors, "}");
}

static inline bool IsAlias(const std::string &lexeme)
{
    bool is_alias = false;

    if (("BLOCK" == lexeme) || ("SWITCH" == lexeme) || ("LIGHT" == lexeme) || ("CROSSING" == lexeme))
    {
        is_alias = true;
    }

    return is_alias;
}

static inline lexer::Token GetToken(std::deque<lexer::Token> &tokens)
{
    lexer::Token token = tokens.front();

    tokens.pop_front();

    return token;
}

static inline void AddUnexpectedEndError(std::deque<lexer::Token> &tokens, std::deque<Error> &errors)
{
    errors.emplace_back(ErrorType::ERRORTYPE_UNEXPECTED_END, tokens.back());
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

static inline bool IsElseNext(std::deque<lexer::Token> &tokens)
{
    bool is_else_next = false;

    if (!tokens.empty() && ("ELSE" == tokens.front().lexeme))
    {
        is_else_next = true;
    }

    return is_else_next;
}

} // namespace plc_compiler::parser