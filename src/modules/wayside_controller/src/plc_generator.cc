#include "plc_generator.h"

#include <ranges>
#include <string>
#include <variant>
#include <vector>

#include "plc_parser.h"
#include "plc_program.pb.h"

namespace plc_compiler::generator
{

using InstructionArgument = uint32_t;
using Id                  = struct Id;

static const InstructionArgument kRegister0 = 0;
static const InstructionArgument kRegister1 = 1;

struct Id
{
    public:
        Id(const std::string &lexeme, const wayside_controller::SignalId signal);
        std::string lexeme;
        wayside_controller::SignalId signal;
};

static bool AddInstructions(parser::SharedStatementAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors);
static bool AddSetInstructions(parser::SharedSetAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors);
static bool AddIfInstructions(parser::SharedIfAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors);
static bool AddExpressionInstructions(parser::SharedExpressionAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors);
static bool AddLeftOperandInstructions(parser::SharedExpressionAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors);
static bool AddRightOperandInstructions(parser::SharedExpressionAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors);
static bool AddBodyInstructions(parser::SharedBodyAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors);
static bool AddElseInstructions(parser::SharedElseAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors);
static inline void AddId(std::vector<Id> &ids, const std::string &lexeme, const wayside_controller::SignalId signal);
static inline void RemoveId(std::vector<Id> &ids);
static inline bool GetSignalFromId(std::vector<Id> &ids, const std::string &id_string, wayside_controller::SignalId &signal);

Error::Error(parser::SharedIdAstNode node) : node(node)
{
}

Error::Error(parser::SharedExpressionAstNode node) : node(node)
{
}

Id::Id(const std::string &lexeme, const wayside_controller::SignalId signal) : lexeme(lexeme), signal(signal)
{
}

std::ostream& operator<<(std::ostream& stream, const Error& error)
{
    stream << "Semantic error: ";

    std::visit([&stream](auto &&arg){
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, parser::SharedIdAstNode>)
            {
                stream << "Invalid ID " << arg;
            }
            else if constexpr (std::is_same_v<T, parser::SharedExpressionAstNode>)
            {
                stream << "Illegal operation " << arg;
            }
            else
            {
                stream << "Unknown";
            }

        }, error.node);

    return stream;
}

bool GenerateCode(parser::SharedStatementAstNode root, std::ostream &stream, std::deque<Error> &errors)
{
    bool                            generated = false;
    parser::SharedStatementAstNode  node      = root;
    std::vector<Id>                 ids;
    controller_messages::PlcProgram plc_program;

    while (nullptr != node)
    {
        generated = AddInstructions(node, plc_program, ids, errors);
        node      = node->next_statement;

        if (!generated)
        {
            break;
        }
    }

    if (generated)
    {
        generated = plc_program.SerializeToOstream(&stream);
    }

    return generated;
}

static bool AddInstructions(parser::SharedStatementAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors)
{
    bool instructions_added = false;

    if (nullptr == node)
    {
        // Do not dereference nullptr, do nothing
    }
    else
    {
        std::visit([&plc_program, &ids, &errors, &instructions_added](auto &&arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, parser::SharedAliasAstNode>)
                {
                    AddId(ids, arg->id_node->id, arg->signal_node->signal);
                    instructions_added = true;
                }
                else if constexpr (std::is_same_v<T, parser::SharedSetAstNode>)
                {
                    instructions_added = AddSetInstructions(arg, plc_program, ids, errors);
                }
                else if constexpr (std::is_same_v<T, parser::SharedIfAstNode>)
                {
                    instructions_added = AddIfInstructions(arg, plc_program, ids, errors);
                }

            }, node->node);
    }

    return instructions_added;
}

static bool AddSetInstructions(parser::SharedSetAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors)
{
    bool                         instructions_added = false;
    wayside_controller::SignalId output_signal      = 0;

    if (nullptr == node)
    {
        // Do not dereference nullptr, do nothing
    }
    else
    {
        bool valid_signal = false;

        std::visit([&plc_program, &ids, &output_signal, &valid_signal, &errors](auto &&arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, parser::SharedIdAstNode>)
                {
                    valid_signal = GetSignalFromId(ids, arg->id, output_signal);

                    if (!valid_signal)
                    {
                        errors.emplace_back(arg);
                    }
                }
                else if constexpr (std::is_same_v<T, parser::SharedSignalAstNode>)
                {
                    output_signal = arg->signal;
                    valid_signal  = true;
                }

            }, node->signal);


        if (valid_signal)
        {
            controller_messages::Instruction *instruction = plc_program.add_instructions();

            instruction->set_instruction_code(controller_messages::InstructionCode::INSTRUCTION_CODE_WRITE_SIGNAL);
            instruction->set_argument_0(static_cast<InstructionArgument>(output_signal));
            instruction->set_argument_1(static_cast<InstructionArgument>(node->logic_level_node->logic_level));

            instructions_added = true;
        }
    }

    return instructions_added;
}

static bool AddIfInstructions(parser::SharedIfAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors)
{
    bool instructions_added = false;

    if (nullptr == node)
    {
        // Do not dereference nullptr, do nothing
    }
    else if (!AddExpressionInstructions(node->expression_node, plc_program, ids, errors))
    {
        // Failed generate instructions for expression, do nothing
    }
    else
    {
        controller_messages::Instruction *branch_not_if_instruction = plc_program.add_instructions();
        uint32_t                          instruction_count         = static_cast<uint32_t>(plc_program.instructions_size());

        instructions_added = AddBodyInstructions(node->body_node, plc_program, ids, errors);

        branch_not_if_instruction->set_instruction_code(controller_messages::InstructionCode::INSTRUCTION_CODE_BRANCH_NOT_IF);
        branch_not_if_instruction->set_argument_0(kRegister0);
        branch_not_if_instruction->set_argument_1(static_cast<uint32_t>(plc_program.instructions_size()) - instruction_count);

        if (node->has_else_node && instructions_added)
        {
            controller_messages::Instruction *branch_unconditional_instruction = plc_program.add_instructions();
            instruction_count = static_cast<uint32_t>(plc_program.instructions_size());

            instructions_added = AddElseInstructions(node->else_node, plc_program, ids, errors);

            branch_unconditional_instruction->set_instruction_code(controller_messages::InstructionCode::INSTRUCTION_CODE_BRANCH_UNDCONDITIONAL);
            branch_not_if_instruction->set_argument_0(static_cast<uint32_t>(plc_program.instructions_size()) - instruction_count);
        }
    }

    return instructions_added;
}

static bool AddExpressionInstructions(parser::SharedExpressionAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors)
{
    bool instructions_added = false;

    if (nullptr == node)
    {
        // Do not dereference nullptr, do nothing
    }
    else
    {
        if (!AddLeftOperandInstructions(node, plc_program, ids, errors))
        {
            // Failed to add instructions for left operand, do nothing
        }
        else if (!AddRightOperandInstructions(node, plc_program, ids, errors))
        {
            // Failed to add instructions for left operand, do nothing
        }
        else
        {
            controller_messages::Instruction *instruction = plc_program.add_instructions();
            instruction->set_argument_0(kRegister0);
            instruction->set_argument_1(kRegister0);
            instruction->set_argument_2(kRegister1);

            switch (node->boolean_operator)
            {
            case parser::ExpressionAstNode::Operator::OPERATOR_COMPARISON:
                instruction->set_instruction_code(controller_messages::InstructionCode::INSTRUCTION_CODE_EQUALS);
                instructions_added = true;
                break;
            case parser::ExpressionAstNode::Operator::OPERATOR_AND:
                instruction->set_instruction_code(controller_messages::InstructionCode::INSTRUCTION_CODE_AND);
                instructions_added = true;
                break;
            case parser::ExpressionAstNode::Operator::OPERATOR_OR:
                instruction->set_instruction_code(controller_messages::InstructionCode::INSTRUCTION_CODE_OR);
                instructions_added = true;
                break;
            default:
                instruction->set_instruction_code(controller_messages::InstructionCode::INSTRUCTION_CODE_NOOP);
                errors.emplace_back(node);
                break;
            }
        }
    }

    return instructions_added;
}

static bool AddLeftOperandInstructions(parser::SharedExpressionAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors)
{
    bool instructions_added = false;

    if (nullptr == node)
    {
        // Do not dereference nullptr, do nothing
    }
    else
    {
        std::visit([&plc_program, &ids, &errors, &instructions_added](auto &&arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, parser::SharedIdAstNode>)
                {
                    wayside_controller::SignalId signal = 0;
                    instructions_added                  = GetSignalFromId(ids, arg->id, signal);

                    if (!instructions_added)
                    {
                        errors.emplace_back(arg);
                    }
                    else
                    {
                        controller_messages::Instruction *instruction = plc_program.add_instructions();

                        instruction->set_instruction_code(controller_messages::InstructionCode::INSTRUCTION_CODE_READ_SIGNAL);
                        instruction->set_argument_0(kRegister0);
                        instruction->set_argument_1(static_cast<uint32_t>(signal));
                    }
                }
                else if constexpr (std::is_same_v<T, parser::SharedSignalAstNode>)
                {
                    controller_messages::Instruction *instruction = plc_program.add_instructions();

                    instruction->set_instruction_code(controller_messages::InstructionCode::INSTRUCTION_CODE_READ_SIGNAL);
                    instruction->set_argument_0(kRegister0);
                    instruction->set_argument_1(static_cast<uint32_t>(arg->signal));

                    instructions_added = true;
                }
                else if constexpr (std::is_same_v<T, parser::SharedExpressionAstNode>)
                {
                    instructions_added                            = AddExpressionInstructions(arg, plc_program, ids, errors);
                    controller_messages::Instruction *instruction = plc_program.add_instructions();

                    instruction->set_instruction_code(controller_messages::InstructionCode::INSTRUCTION_CODE_PUSH);
                    instruction->set_argument_0(kRegister0);
                }

            }, node->left_operand);
    }

    return instructions_added;
}

static bool AddRightOperandInstructions(parser::SharedExpressionAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors)
{
    bool instructions_added = false;

    if (nullptr == node)
    {
        // Do not dereference nullptr, do nothing
    }
    else
    {
        std::visit([&plc_program, &ids, &errors, &instructions_added](auto &&arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, parser::SharedLogicLevelAstNode>)
                {
                    controller_messages::Instruction *instruction = plc_program.add_instructions();

                    instruction->set_instruction_code(controller_messages::InstructionCode::INSTRUCTION_CODE_READ_IMMEDIATE);
                    instruction->set_argument_0(kRegister1);
                    instruction->set_argument_1(static_cast<uint32_t>(arg->logic_level));

                    instructions_added = true;
                }
                else if constexpr (std::is_same_v<T, parser::SharedExpressionAstNode>)
                {
                    instructions_added                            = AddExpressionInstructions(arg, plc_program, ids, errors);
                    controller_messages::Instruction *instruction = plc_program.add_instructions();

                    instruction->set_instruction_code(controller_messages::InstructionCode::INSTRUCTION_CODE_POP);
                    instruction->set_argument_0(kRegister1);
                }

            }, node->right_operand);
    }

    return instructions_added;
}

static bool AddBodyInstructions(parser::SharedBodyAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors)
{
    bool instructions_added = false;

    if (nullptr == node)
    {
        // Do not dereference nullptr, do nothing
    }
    else
    {
        parser::SharedStatementAstNode statement_node = node->statement_node;
        instructions_added = true;
        size_t id_count = ids.size();

        while ((nullptr != statement_node) && instructions_added)
        {
            instructions_added = AddInstructions(statement_node, plc_program, ids, errors);
            statement_node     = statement_node->next_statement;
        }

        // Remove any IDs added in the body to ensure they are scoped only to the body
        for (size_t i = ids.size(); i > id_count; i--)
        {
            RemoveId(ids);
        }
    }

    return instructions_added;
}

static bool AddElseInstructions(parser::SharedElseAstNode node, controller_messages::PlcProgram &plc_program, std::vector<Id> &ids, std::deque<Error> &errors)
{
    bool instructions_added = false;

    if (nullptr == node)
    {
        // Do not dereference nullptr, do nothing
    }
    else
    {
        std::visit([&plc_program, &ids, &errors, &instructions_added](auto &&arg)
            {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, parser::SharedIfAstNode>)
                {
                    instructions_added = AddIfInstructions(arg, plc_program, ids, errors);
                }
                else if constexpr (std::is_same_v<T, parser::SharedBodyAstNode>)
                {
                    instructions_added = AddBodyInstructions(arg, plc_program, ids, errors);
                }

            }, node->predicate_node);
    }

    return instructions_added;
}

static inline void AddId(std::vector<Id> &ids, const std::string &lexeme, const wayside_controller::SignalId signal)
{
    ids.emplace_back(lexeme, signal);
}

static inline void RemoveId(std::vector<Id> &ids)
{
    ids.pop_back();
}

static inline bool GetSignalFromId(std::vector<Id> &ids, const std::string &id_string, wayside_controller::SignalId &signal)
{
    bool found = false;

    if (std::vector<Id>::const_iterator id = std::ranges::find_if(ids, [&id_string](const Id &id){
            return id.lexeme == id_string;
        }); ids.end() != id)
    {
        signal = id->signal;
        found  = true;
    }

    return found;
}

} // namespace plc_compiler::generator