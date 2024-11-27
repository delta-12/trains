#include "plc_compiler.h"

#include <fstream>
#include <deque>

#include "plc_lexer.h"

namespace plc_compiler
{

bool Compile(std::filesystem::path &file_path)
{
    bool                     compiled = false;
    std::deque<lexer::Token> tokens;
    std::deque<lexer::Error> errors;

    if (!std::filesystem::exists(file_path))
    {
        // File does not exist, do nothing
    }
    else
    {
        std::ifstream program_file(file_path);

        if (!program_file.is_open())
        {
            // Failed to open file, do nothing
        }
        else
        {
            lexer::Lexer(program_file, tokens, errors);


            program_file.close();
        }
    }

    return compiled;
}

} // namespace plc_compiler