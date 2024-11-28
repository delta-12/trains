#include "plc_compiler.h"

#include <fstream>
#include <deque>

#include "plc_lexer.h"
#include "plc_parser.h"
#include "types.h"

namespace plc_compiler
{

types::Error Compile(std::filesystem::path &file_path)
{
    types::Error              error = types::Error::ERROR_NONE;
    std::deque<lexer::Token>  tokens;
    std::deque<lexer::Error>  lexer_errors;
    std::deque<parser::Error> parser_errors;

    if (!std::filesystem::exists(file_path))
    {
        error = types::Error::ERROR_FILE_NO_EXIST;
    }
    else
    {
        std::ifstream program_file(file_path);

        if (!program_file.is_open())
        {
            error = types::Error::ERROR_IO;
        }
        else if (!lexer::Lexer(program_file, tokens, lexer_errors) || !parser::Parse(tokens, parser_errors))
        {
            // TODO print errors

            error = types::Error::ERROR_INVALID_FORMAT;
            program_file.close();
        }
        else
        {
            // TODO continue parsing
        }
    }

    return error;
}

} // namespace plc_compiler