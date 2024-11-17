/*****************************************************************************
* @file plc_lexer.h
*
* @brief Implements the lexer for the PLC program compiler.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_LEXER_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_LEXER_H

#include <array>
#include <cstddef>
#include <istream>
#include <string>
#include <vector>

namespace plc_compiler::lexer
{

using Token = struct Token;
using Error = struct Error;

enum class TokenType
{
    TOKENTYPE_KEYWORD,
    TOKENTYPE_ID,
    TOKENTYPE_INPUT_SIGNAL,
    TOKENTYPE_OUTPUT_SIGNAL,
    TOKENTYPE_SYMBOL,
    TOKENTYPE_MAX
};

enum class ErrorType
{
    ERRORTYPE_INVALID_INPUT,
    ERRORTYPE_ILLEGAL_SYMBOL,
    ERRORTYPE_MAX
};

struct Token
{
    public:
        Token(void);
        Token(const TokenType token_type, const std::string &lexeme);
        bool operator==(const Token &token_right) const;
        TokenType token_type;
        std::string lexeme;
};

struct Error
{
    public:
        Error(void);
        Error(const ErrorType error_type, const std::string &lexeme);
        bool operator==(const Error &error_right) const;
        ErrorType error_type;
        std::string lexeme;
};

bool Lexer(std::istream &input, std::vector<Token> &tokens, std::vector<Error> &errors);

} // namespace plc_compiler::lexer

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_PLC_LEXER_H