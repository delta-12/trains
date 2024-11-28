/*****************************************************************************
* @file plc_compiler_tests.cc
*
* @brief Unit testing for the PLC compiler.
*****************************************************************************/

#include <sstream>
#include <deque>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock/gmock-matchers.h>

#include "plc_lexer.h"
#include "plc_parser.h"

static const plc_compiler::lexer::Token kValidInputTokens[] = {
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "BLOCK"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_ID, "block_0"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "="},
    {plc_compiler::lexer::TokenType::TOKENTYPE_INPUT_SIGNAL, "IN_0"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ";"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "BLOCK"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_ID, "block_1"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "="},
    {plc_compiler::lexer::TokenType::TOKENTYPE_INPUT_SIGNAL, "IN_1"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ";"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "SWITCH"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_ID, "switch_0"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "="},
    {plc_compiler::lexer::TokenType::TOKENTYPE_OUTPUT_SIGNAL, "OUT_0"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ";"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "CROSSING"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_ID, "Crossing_0"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "="},
    {plc_compiler::lexer::TokenType::TOKENTYPE_OUTPUT_SIGNAL, "OUT_1"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ";"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "IF"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "("},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "("},
    {plc_compiler::lexer::TokenType::TOKENTYPE_ID, "block_0"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "=="},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "HIGH"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ")"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "||"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "("},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "HIGH"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "=="},
    {plc_compiler::lexer::TokenType::TOKENTYPE_INPUT_SIGNAL, "IN_2"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ")"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "||"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "("},
    {plc_compiler::lexer::TokenType::TOKENTYPE_ID, "block_1"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "=="},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "HIGH"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ")"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ")"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "{"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "SET"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_ID, "Crossing_0"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "HIGH"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ";"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "}"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "ELSE"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "{"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "SET"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_ID, "Crossing_0"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "LOW"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ";"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "}"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "IF"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "("},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "("},
    {plc_compiler::lexer::TokenType::TOKENTYPE_ID, "block_0"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "=="},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "HIGH"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ")"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "&&"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "("},
    {plc_compiler::lexer::TokenType::TOKENTYPE_ID, "block_1"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "=="},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "LOW"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ")"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ")"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "{"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "SET"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_ID, "switch_0"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "LOW"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ";"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "}"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "ELSEIF"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "("},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "("},
    {plc_compiler::lexer::TokenType::TOKENTYPE_ID, "block_0"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "=="},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "LOW"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ")"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "&&"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "("},
    {plc_compiler::lexer::TokenType::TOKENTYPE_ID, "block_1"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "=="},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "HIGH"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ")"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ")"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "{"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "SET"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_ID, "switch_0"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD, "HIGH"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, ";"},
    {plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL, "}"}
};

static const plc_compiler::lexer::Error kInvalidInputErrors[] = {
    {plc_compiler::lexer::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL, "@"},
    {plc_compiler::lexer::ErrorType::ERRORTYPE_INVALID_INPUT, "_"},
    {plc_compiler::lexer::ErrorType::ERRORTYPE_INVALID_INPUT, "8"},
    {plc_compiler::lexer::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL, "!"},
    {plc_compiler::lexer::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL, "|"},
    {plc_compiler::lexer::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL, "&"},
    {plc_compiler::lexer::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL, "*"},
    {plc_compiler::lexer::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL, "%"},
    {plc_compiler::lexer::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL, "%"},
    {plc_compiler::lexer::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL, "#"},
};

TEST(PlcCompilerTests, LexerTokenEquality)
{
    plc_compiler::lexer::Token token_a, token_b;

    ASSERT_TRUE(token_a == token_b);
    ASSERT_TRUE(token_b == token_a);

    token_a.token_type = plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD;
    token_a.lexeme     = "BLOCK";
    token_b.token_type = plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD;
    token_b.lexeme     = "SWITCH";
    ASSERT_FALSE(token_a == token_b);
    ASSERT_FALSE(token_b == token_a);

    token_a.token_type = plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD;
    token_a.lexeme     = "BLOCK";
    token_b.token_type = plc_compiler::lexer::TokenType::TOKENTYPE_ID;
    token_b.lexeme     = "BLOCK";
    ASSERT_FALSE(token_a == token_b);
    ASSERT_FALSE(token_b == token_a);

    token_a.token_type = plc_compiler::lexer::TokenType::TOKENTYPE_KEYWORD;
    token_a.lexeme     = "BLOCK";
    token_b.token_type = plc_compiler::lexer::TokenType::TOKENTYPE_ID;
    token_b.lexeme     = "switch_0";
    ASSERT_FALSE(token_a == token_b);
    ASSERT_FALSE(token_b == token_a);

    token_b = token_a;
    ASSERT_TRUE(token_a == token_b);
    ASSERT_TRUE(token_b == token_a);
}

TEST(PlcCompilerTests, LexerErrorEquality)
{
    plc_compiler::lexer::Error error_a, error_b;

    ASSERT_TRUE(error_a == error_b);
    ASSERT_TRUE(error_b == error_a);

    error_a.error_type = plc_compiler::lexer::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL;
    error_a.lexeme     = "@";
    error_b.error_type = plc_compiler::lexer::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL;
    error_b.lexeme     = "!";
    ASSERT_FALSE(error_a == error_b);
    ASSERT_FALSE(error_b == error_a);

    error_a.error_type = plc_compiler::lexer::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL;
    error_a.lexeme     = "0";
    error_b.error_type = plc_compiler::lexer::ErrorType::ERRORTYPE_INVALID_INPUT;
    error_b.lexeme     = "0";
    ASSERT_FALSE(error_a == error_b);
    ASSERT_FALSE(error_b == error_a);

    error_a.error_type = plc_compiler::lexer::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL;
    error_a.lexeme     = "%";
    error_b.error_type = plc_compiler::lexer::ErrorType::ERRORTYPE_INVALID_INPUT;
    error_b.lexeme     = "_";
    ASSERT_FALSE(error_a == error_b);
    ASSERT_FALSE(error_b == error_a);

    error_b = error_a;
    ASSERT_TRUE(error_a == error_b);
    ASSERT_TRUE(error_b == error_a);
}

TEST(PlcCompilerTests, ParserErrorEquality)
{
    plc_compiler::parser::Error error_a, error_b;

    ASSERT_TRUE(error_a == error_b);
    ASSERT_TRUE(error_b == error_a);

    error_a.error_type       = plc_compiler::parser::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL;
    error_a.token.token_type = plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL;
    error_a.token.lexeme     = "@";
    error_b.error_type       = plc_compiler::parser::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL;
    error_b.token.token_type = plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL;
    error_b.token.lexeme     = "!";
    ASSERT_FALSE(error_a == error_b);
    ASSERT_FALSE(error_b == error_a);

    error_a.error_type       = plc_compiler::parser::ErrorType::ERRORTYPE_INVALID_SIGNAL;
    error_a.token.token_type = plc_compiler::lexer::TokenType::TOKENTYPE_INPUT_SIGNAL;
    error_a.token.lexeme     = "IN_0";
    error_b.error_type       = plc_compiler::parser::ErrorType::ERRORTYPE_INVALID_INPUT;
    error_b.token.token_type = plc_compiler::lexer::TokenType::TOKENTYPE_INPUT_SIGNAL;
    error_b.token.lexeme     = "IN_0";
    ASSERT_FALSE(error_a == error_b);
    ASSERT_FALSE(error_b == error_a);

    error_a.error_type       = plc_compiler::parser::ErrorType::ERRORTYPE_ILLEGAL_SYMBOL;
    error_a.token.token_type = plc_compiler::lexer::TokenType::TOKENTYPE_SYMBOL;
    error_a.token.lexeme     = "%";
    error_b.error_type       = plc_compiler::parser::ErrorType::ERRORTYPE_INVALID_INPUT;
    error_b.token.token_type = plc_compiler::lexer::TokenType::TOKENTYPE_ID;
    error_b.token.lexeme     = "_";
    ASSERT_FALSE(error_a == error_b);
    ASSERT_FALSE(error_b == error_a);

    error_b = error_a;
    ASSERT_TRUE(error_a == error_b);
    ASSERT_TRUE(error_b == error_a);
}

TEST(PlcCompilerTests, LexerValidInput)
{
    std::deque<plc_compiler::lexer::Token>  tokens;
    std::deque<plc_compiler::lexer::Error>  lexer_errors;
    std::deque<plc_compiler::parser::Error> parser_errors;
    std::stringstream                       input;
    input << "BLOCK block_0 = IN_0;\n";
    input << "BLOCK block_1=IN_1;\n";
    input << "SWITCH switch_0= OUT_0;\n";
    input << "CROSSING Crossing_0 =OUT_1;\n";
    input << "\n\r\t \n";
    input << "IF ((block_0 == HIGH\n) ||\t (HIGH == IN_2)|| ( block_1 == HIGH))\n";
    input << "{\n";
    input << "\tSET Crossing_0 HIGH;\n";
    input << "}\n";
    input << "ELSE{SET Crossing_0 LOW;\r\n}\n";
    input << "\n\n";
    input << "IF ((block_0 == HIGH)&&(block_1 == LOW)){\n";
    input << "    SET switch_0 LOW;\n";
    input << "}ELSEIF ((block_0==LOW) && (block_1 == HIGH))\n";
    input << "{SET switch_0 HIGH;}\n";

    // Lexer
    ASSERT_TRUE(plc_compiler::lexer::Lexer(input, tokens, lexer_errors));
    ASSERT_EQ(0, lexer_errors.size());
    ASSERT_THAT(tokens, testing::ElementsAreArray(kValidInputTokens));
}

TEST(PlcCompilerTests, ParserValidInput)
{
    std::deque<plc_compiler::lexer::Token>  tokens;
    std::deque<plc_compiler::lexer::Error>  lexer_errors;
    std::deque<plc_compiler::parser::Error> parser_errors;
    std::stringstream                       input;
    input << "BLOCK block_0 = IN_0;\n";
    input << "BLOCK block_1=IN_1;\n";
    input << "SWITCH switch_0= OUT_0;\n";
    input << "CROSSING Crossing_0 =OUT_1;\n";
    input << "\n\r\t \n";
    // input << "IF ((block_0 == HIGH\n) ||\t (IN_2 == HIGH)|| ( block_1 == HIGH))\n";
    input << "IF ((block_0 == HIGH\n) ||\t (IN_2 == HIGH))\n";
    input << "{\n";
    input << "\tSET Crossing_0 HIGH;\n";
    input << "}\n";
    input << "ELSE{SET Crossing_0 LOW;\r\n}\n";
    input << "\n\n";
    input << "IF ((block_0 == HIGH)&&(block_1 == LOW)){\n";
    input << "    SET switch_0 LOW;\n";
    input << "}ELSE IF ((block_0==LOW) && (block_1 == HIGH))\n";
    input << "{SET switch_0 HIGH;}\n";

    // Lexer
    ASSERT_TRUE(plc_compiler::lexer::Lexer(input, tokens, lexer_errors));
    ASSERT_EQ(0, lexer_errors.size());

    // Parser
    plc_compiler::parser::SharedStatementAstNode node = plc_compiler::parser::Parse(tokens, parser_errors);
    ASSERT_NE(nullptr, node);
    // ASSERT_EQ(0, parser_errors.size());
    for (const plc_compiler::parser::Error &error : parser_errors)
    {
        std::cout << error;
    }
    // while (node != nullptr)
    // {

    // }
}

TEST(PlcCompilerTests, InvalidInput)
{
    std::deque<plc_compiler::lexer::Token> tokens;
    std::deque<plc_compiler::lexer::Error> errors;
    std::stringstream                      input;
    input << "BLOCK @block_0 = IN_0;\n";
    input << "BLOCK _block_1=IN_1;\n";
    input << "SWITCH 8switch_0= OUT_0;\n";
    input << "CROSSING !Crossing_0 =|OUT_1;\n";
    input << "\n\r\t \n";
    input << "IF ((block_0 == HIGH\n) ||\t (HIGH == IN_2)||& ( block_1 == HIGH))\n";
    input << "{\n";
    input << "\tSET Crossing_0 HIGH;\n";
    input << "}\n";
    input << "ELSE{SET Crossing_0* LOW;\r\n}\n";
    input << "\n\n";
    input << "IF ((block_0 == HIGH)&&(block_1 == LOW)){\n";
    input << "    SET switch_0 LOW%%;\n";
    input << "}ELSEIF ((block_0==LOW) && (block_1 == HIGH))\n";
    input << "{SET #switch_0 HIGH;}\n";

    ASSERT_FALSE(plc_compiler::lexer::Lexer(input, tokens, errors));
    ASSERT_NE(0, errors.size());
    ASSERT_THAT(tokens, testing::ElementsAreArray(kValidInputTokens));
    ASSERT_THAT(errors, testing::ElementsAreArray(kInvalidInputErrors));
}