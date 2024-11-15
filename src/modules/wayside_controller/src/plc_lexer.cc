#include "plc_lexer.h"

#include <functional>
#include <ranges>

#include "wayside_controller.h"

namespace plc_compiler
{

namespace lexer
{

enum class DfaState
{
    DFASTATE_INITIAL,
    DFASTATE_WHITESPACE,
    DFASTATE_INPUT,
    DFASTATE_KEYWORD_ID_SIGNAL,
    DFASTATE_INVALID_INPUT,
    DFASTATE_ILLEGAL_SYMBOL,
    DFASTATE_SYMBOL_SINGLE_EQUALS,
    DFASTATE_SYMBOL_DOUBLE_EQUALS,
    DFASTATE_SYMBOL_AMPERSAND,
    DFASTATE_SYMBOL_VERTICAL_BAR,
    DFASTATE_VALID_SYMBOL,
    DFASTATE_MAX
};

enum class DfaIndex
{
    DFAINDEX_WHITESPACE,
    DFAINDEX_LETTER,
    DFAINDEX_NUMBER,
    DFAINDEX_UNDERSCORE,
    DFAINDEX_EQUALS,
    DFAINDEX_AMPERSAND,
    DFAINDEX_VERTICAL_BAR,
    DFAINDEX_VALID_SYMBOL,
    DFAINDEX_INVALID_SYMBOL
};

static const size_t kCharacterTypes            = 9;
static const size_t kDefaultTokenCharacterSize = 16;
static const size_t kKeywordCount              = 10;

static const std::array<const char *const, kKeywordCount> kKeywords = {
    "HIGH",
    "LOW",
    "BLOCK",
    "SWITCH",
    "LIGHT",
    "CROSSING",
    "IF",
    "ELSEIF",
    "ELSE",
    "SET"};

static const std::array<const char *const, wayside_controller::kTotalInputs> kInputSignals = {
    "IN_0",
    "IN_1",
    "IN_2",
    "IN_3",
    "IN_4",
    "IN_5",
    "IN_6",
    "IN_7",
    "IN_8",
    "IN_9",
    "IN_10",
    "IN_11",
    "IN_12",
    "IN_13",
    "IN_14",
    "IN_15",
    "IN_16",
    "IN_17",
    "IN_18",
    "IN_19",
    "IN_20",
    "IN_21",
    "IN_22",
    "IN_23",
    "IN_24",
    "IN_25",
    "IN_26",
    "IN_27",
    "IN_28",
    "IN_29",
    "IN_30",
    "IN_31",
    "IN_32",
    "IN_33",
    "IN_34",
    "IN_35",
    "IN_36",
    "IN_37",
    "IN_38",
    "IN_39",
    "IN_40",
    "IN_41",
    "IN_42",
    "IN_43",
    "IN_44",
    "IN_45",
    "IN_46",
    "IN_47",
    "IN_48",
    "IN_49",
    "IN_50",
    "IN_51",
    "IN_52",
    "IN_53",
    "IN_54",
    "IN_55",
    "IN_56",
    "IN_57",
    "IN_58",
    "IN_59",
    "IN_60",
    "IN_61",
    "IN_62",
    "IN_63",
    "IN_64",
    "IN_65",
    "IN_66",
    "IN_67",
    "IN_68",
    "IN_69",
    "IN_70",
    "IN_71",
    "IN_72",
    "IN_73",
    "IN_74",
    "IN_75",
    "IN_76",
    "IN_77",
    "IN_78",
    "IN_79",
    "IN_80",
    "IN_81",
    "IN_82",
    "IN_83",
    "IN_84"
};

static const std::array<const char *const, wayside_controller::kTotalOutputs> kOutputSignals = {
    "OUT_0",
    "OUT_1",
    "OUT_2",
    "OUT_3",
    "OUT_4",
    "OUT_5",
    "OUT_6",
    "OUT_7",
    "OUT_8",
    "OUT_9",
    "OUT_10",
    "OUT_11",
    "OUT_12",
    "OUT_13",
    "OUT_14",
    "OUT_15",
    "OUT_16",
    "OUT_17",
    "OUT_18",
    "OUT_19",
    "OUT_20",
    "OUT_21",
    "OUT_22"
};

// TODO NNF-239 singular end state with all next states pointing back to initial state
static const std::array<std::array<DfaState, kCharacterTypes>, static_cast<size_t>(DfaState::DFASTATE_MAX)> kDfaStateTable = {{
    // Whitespace                          Letter                                Number                                Underscore                            =                                        &                                     |                                       (, ), {, }, ;                         Other symbols
    {DfaState::DFASTATE_WHITESPACE,        DfaState::DFASTATE_INPUT,             DfaState::DFASTATE_INVALID_INPUT,     DfaState::DFASTATE_INVALID_INPUT,     DfaState::DFASTATE_SYMBOL_SINGLE_EQUALS, DfaState::DFASTATE_SYMBOL_AMPERSAND,  DfaState::DFASTATE_SYMBOL_VERTICAL_BAR, DfaState::DFASTATE_VALID_SYMBOL,      DfaState::DFASTATE_ILLEGAL_SYMBOL},
    {DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,              DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,             DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL},
    {DfaState::DFASTATE_KEYWORD_ID_SIGNAL, DfaState::DFASTATE_INPUT,             DfaState::DFASTATE_INPUT,             DfaState::DFASTATE_INPUT,             DfaState::DFASTATE_KEYWORD_ID_SIGNAL,    DfaState::DFASTATE_KEYWORD_ID_SIGNAL, DfaState::DFASTATE_KEYWORD_ID_SIGNAL,   DfaState::DFASTATE_KEYWORD_ID_SIGNAL, DfaState::DFASTATE_KEYWORD_ID_SIGNAL},
    {DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,              DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,             DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL},
    {DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,              DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,             DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL},
    {DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,              DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,             DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL},
    {DfaState::DFASTATE_VALID_SYMBOL,      DfaState::DFASTATE_VALID_SYMBOL,      DfaState::DFASTATE_VALID_SYMBOL,      DfaState::DFASTATE_VALID_SYMBOL,      DfaState::DFASTATE_SYMBOL_DOUBLE_EQUALS, DfaState::DFASTATE_VALID_SYMBOL,      DfaState::DFASTATE_VALID_SYMBOL,        DfaState::DFASTATE_VALID_SYMBOL,      DfaState::DFASTATE_VALID_SYMBOL},
    {DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,              DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,             DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL},
    {DfaState::DFASTATE_ILLEGAL_SYMBOL,    DfaState::DFASTATE_ILLEGAL_SYMBOL,    DfaState::DFASTATE_ILLEGAL_SYMBOL,    DfaState::DFASTATE_ILLEGAL_SYMBOL,    DfaState::DFASTATE_ILLEGAL_SYMBOL,       DfaState::DFASTATE_VALID_SYMBOL,      DfaState::DFASTATE_ILLEGAL_SYMBOL,      DfaState::DFASTATE_ILLEGAL_SYMBOL,    DfaState::DFASTATE_ILLEGAL_SYMBOL},
    {DfaState::DFASTATE_ILLEGAL_SYMBOL,    DfaState::DFASTATE_ILLEGAL_SYMBOL,    DfaState::DFASTATE_ILLEGAL_SYMBOL,    DfaState::DFASTATE_ILLEGAL_SYMBOL,    DfaState::DFASTATE_ILLEGAL_SYMBOL,       DfaState::DFASTATE_ILLEGAL_SYMBOL,    DfaState::DFASTATE_VALID_SYMBOL,        DfaState::DFASTATE_ILLEGAL_SYMBOL,    DfaState::DFASTATE_ILLEGAL_SYMBOL},
    {DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,              DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL,             DfaState::DFASTATE_INITIAL,           DfaState::DFASTATE_INITIAL}
}};

static DfaIndex GetIndex(const char input);
static bool IsLetter(const char input);
static TokenType GetInputType(const std::string &input);

Token::Token(void) : token_type(TokenType::TOKENTYPE_MAX), lexeme("")
{
}

Token::Token(const TokenType token_type, const std::string &lexeme) : token_type(token_type), lexeme(lexeme)
{
}

bool Token::operator==(const Token& token_right) const
{
    return (0 == token_right.lexeme.compare(lexeme)) && (token_right.token_type == token_type);
}

Error::Error(void) : error_type(ErrorType::ERRORTYPE_MAX), lexeme("")
{
}

Error::Error(const ErrorType error_type, const std::string &lexeme) : error_type(error_type), lexeme(lexeme)
{
}

bool Error::operator==(const Error &error_right) const
{
    return (0 == error_right.lexeme.compare(lexeme)) && (error_right.error_type == error_type);
}

bool Lexer(std::istream &input, std::vector<Token> &tokens, std::vector<Error> &errors)
{
    DfaState    state = DfaState::DFASTATE_INITIAL;
    std::string lexeme;

    lexeme.reserve(kDefaultTokenCharacterSize);
    tokens.clear();
    errors.clear();

    while (EOF != input.peek())
    {
        DfaState next_state = kDfaStateTable[static_cast<size_t>(state)][static_cast<size_t>(GetIndex(input.peek()))];

        switch (state)
        {
        case DfaState::DFASTATE_INITIAL:
            lexeme += input.get();
            break;
        case DfaState::DFASTATE_WHITESPACE:
            lexeme.clear();
            break;
        case DfaState::DFASTATE_INPUT:
            if (state == next_state)
            {
                lexeme += input.get();
            }
            break;
        case DfaState::DFASTATE_KEYWORD_ID_SIGNAL:
            tokens.emplace_back(GetInputType(lexeme), lexeme);
            lexeme.clear();
            break;
        case DfaState::DFASTATE_ILLEGAL_SYMBOL:
            errors.emplace_back(ErrorType::ERRORTYPE_ILLEGAL_SYMBOL, lexeme);
            lexeme.clear();
            break;
        case DfaState::DFASTATE_SYMBOL_SINGLE_EQUALS:
            if (DfaState::DFASTATE_SYMBOL_DOUBLE_EQUALS == next_state)
            {
                lexeme += input.get();
            }
            break;
        case DfaState::DFASTATE_SYMBOL_AMPERSAND:
        case DfaState::DFASTATE_SYMBOL_VERTICAL_BAR:
            if (DfaState::DFASTATE_VALID_SYMBOL == next_state)
            {
                lexeme += input.get();
            }
            break;
        case DfaState::DFASTATE_SYMBOL_DOUBLE_EQUALS:
        case DfaState::DFASTATE_VALID_SYMBOL:
            tokens.emplace_back(TokenType::TOKENTYPE_SYMBOL, lexeme);
            lexeme.clear();
            break;
        case DfaState::DFASTATE_INVALID_INPUT:
        default:
            errors.emplace_back(ErrorType::ERRORTYPE_INVALID_INPUT, lexeme);
            lexeme.clear();
            break;
        }

        state = next_state;
    }

    return errors.size() == 0;
}

static DfaIndex GetIndex(const char input)
{
    DfaIndex index = DfaIndex::DFAINDEX_WHITESPACE;

    switch (input)
    {
    // Whitespace
    case ' ':
    case '\t':
    case '\n':
    case '\r':
        break;

    // Number
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        index = DfaIndex::DFAINDEX_NUMBER;
        break;

    // Underscore
    case '_':
        index = DfaIndex::DFAINDEX_UNDERSCORE;
        break;

    // Equals
    case '=':
        index = DfaIndex::DFAINDEX_EQUALS;
        break;

    // Ampersand
    case '&':
        index = DfaIndex::DFAINDEX_AMPERSAND;
        break;

    // Vertical bar
    case '|':
        index = DfaIndex::DFAINDEX_VERTICAL_BAR;
        break;

    // Valid symbol
    case '(':
    case ')':
    case '{':
    case '}':
    case ';':
        index = DfaIndex::DFAINDEX_VALID_SYMBOL;
        break;

    // Letter or invalid symbol
    default:
        if (IsLetter(input))
        {
            index = DfaIndex::DFAINDEX_LETTER;
        }
        else
        {
            index = DfaIndex::DFAINDEX_INVALID_SYMBOL;
        }
    }

    return index;
}

static bool IsLetter(const char input)
{
    bool is_letter = false;

    if ((input >= 'a') && (input <= 'z'))
    {
        is_letter = true;
    }
    else if ((input >= 'A') && (input <= 'Z'))
    {
        is_letter = true;
    }

    return is_letter;
}

static TokenType GetInputType(const std::string &input)
{
    TokenType input_type = TokenType::TOKENTYPE_ID;

    std::function<bool(const char *const comparison_string)> string_compare = [input](const char *const comparison_string)
                                                                              {
                                                                                  return 0 == input.compare(comparison_string);
                                                                              };

    if (kKeywords.end() != std::ranges::find_if(kKeywords, string_compare))
    {
        input_type = TokenType::TOKENTYPE_KEYWORD;
    }
    else if (kInputSignals.end() != std::ranges::find_if(kInputSignals, string_compare))
    {
        input_type = TokenType::TOKENTYPE_INPUT_SIGNAL;
    }
    else if (kOutputSignals.end() != std::ranges::find_if(kOutputSignals, string_compare))
    {
        input_type = TokenType::TOKENTYPE_OUTPUT_SIGNAL;
    }

    return input_type;
}

} // namespace lexer

} // namespace plc_compiler
