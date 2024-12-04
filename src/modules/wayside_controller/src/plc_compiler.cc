#include "plc_compiler.h"

#include <deque>
#include <fstream>
#include <iostream>

#include "plc_lexer.h"
#include "plc_parser.h"
#include "types.h"

namespace plc_compiler
{

types::Error Compile(const std::filesystem::path &file_path)
{
    types::Error error = types::Error::ERROR_NONE;

    if (!std::filesystem::exists(file_path))
    {
        error = types::Error::ERROR_FILE_NO_EXIST;
    }
    else
    {
        std::deque<lexer::Token>  tokens;
        std::deque<lexer::Error>  lexer_errors;
        std::deque<parser::Error> parser_errors;

        std::ifstream program_file(file_path);

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
        else if (!parser::Parse(tokens, parser_errors))
        {
            for (const parser::Error &parser_error : parser_errors)
            {
                std::cout << parser_error << std::endl;
            }

            error = types::Error::ERROR_INVALID_FORMAT;
            program_file.close();
        }
        else
        {
            // TODO NNF-216 continue parsing
        }
    }

    return error;
}

} // namespace plc_compiler