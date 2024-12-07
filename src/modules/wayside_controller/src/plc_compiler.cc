#include "plc_compiler.h"

#include <deque>
#include <fstream>
#include <iostream>
#include <ostream>

#include "plc_generator.h"
#include "plc_lexer.h"
#include "plc_parser.h"
#include "types.h"

namespace plc_compiler
{

types::Error Compile(const std::filesystem::path &file_path, std::ostream &stream)
{
    types::Error error = types::Error::ERROR_NONE;

    if (!std::filesystem::exists(file_path))
    {
        error = types::Error::ERROR_FILE_NO_EXIST;
    }
    else
    {
        std::ifstream                  program_file(file_path);
        parser::SharedStatementAstNode root_node = nullptr;
        std::deque<lexer::Token>       tokens;
        std::deque<lexer::Error>       lexer_errors;
        std::deque<parser::Error>      parser_errors;
        std::deque<generator::Error>   generator_errors;

        if (!program_file.is_open())
        {
            error = types::Error::ERROR_IO;
        }
        else if (!lexer::Lexer(program_file, tokens, lexer_errors))
        {
            for (const lexer::Error &parser_error : lexer_errors)
            {
                std::cout << parser_error << std::endl;
            }

            error = types::Error::ERROR_INVALID_FORMAT;
            program_file.close();
        }
        else if (!parser::Parse(tokens, parser_errors, root_node))
        {
            for (const parser::Error &parser_error : parser_errors)
            {
                std::cout << parser_error << std::endl;
            }

            error = types::Error::ERROR_INVALID_FORMAT;
            program_file.close();
        }
        else if (!generator::GenerateCode(root_node, stream, generator_errors))
        {
            for (const generator::Error &generator_error : generator_errors)
            {
                std::cout << generator_error << std::endl;
            }

            error = types::Error::ERROR_INVALID_FORMAT;
            program_file.close();
        }
    }

    return error;
}

} // namespace plc_compiler