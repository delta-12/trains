#include "plc.h"

#include "wayside_controller.h"

namespace wayside_controller
{

PlcInstruction::PlcInstruction(void) : instruction_code(PlcInstructionCode::PLCINSTRUCTIONCODE_NOOP), argument_0(0), argument_1(0), argument_2(0)
{
}

PlcInstruction::PlcInstruction(const PlcInstructionCode instruction_code, const PlcInstructionArgument argument_0, const PlcInstructionArgument argument_1, const PlcInstructionArgument argument_2)
    : instruction_code(instruction_code), argument_0(argument_0), argument_1(argument_1), argument_2(argument_2)
{
}

Plc::Plc(const std::function<Error(const InputId input, IoSignal &signal)> &get_input, const std::function<Error(const OutputId output, const IoSignal signal)> &set_output)
    : get_input_(get_input), set_output_(set_output)
{
}

Plc::Plc(const std::function<Error(const InputId input, IoSignal &signal)> &get_input, const std::function<Error(const OutputId output, const IoSignal signal)> &set_output,
         const std::vector<PlcInstruction> &instructions)
    : get_input_(get_input), set_output_(set_output), instructions_(instructions)
{
}

void Plc::SetInstructions(const std::vector<PlcInstruction> &instructions)
{
    instructions_ = instructions;
}

uint32_t Plc::GetProgramCounter(void) const
{
    return program_counter_;
}

PlcInstruction Plc::GetInstruction(void) const
{
    return instructions_[program_counter_];
}

bool Plc::Run(void)
{
    PlcInstruction instruction;
    bool           execution_success = true;
    program_counter_ = 0;
    stack_.clear();

    while ((program_counter_ < instructions_.size()) && (true == execution_success))
    {
        instruction = instructions_[program_counter_];

        switch (instruction.instruction_code)
        {
        case PlcInstructionCode::PLCINSTRUCTIONCODE_NOOP:
            execution_success = Noop();
            break;
        case PlcInstructionCode::PLCINSTRUCTIONCODE_READ_IMMEDIATE:
            execution_success = ReadImmediate(instruction.argument_0, instruction.argument_1);
            break;
        case PlcInstructionCode::PLCINSTRUCTIONCODE_READ_SIGNAL:
            execution_success = ReadSignal(instruction.argument_0, instruction.argument_1);
            break;
        case PlcInstructionCode::PLCINSTRUCTIONCODE_WRITE_SIGNAL:
            execution_success = WriteSignal(instruction.argument_0, instruction.argument_1);
            break;
        case PlcInstructionCode::PLCINSTRUCTIONCODE_EQUALS:
            execution_success = Equals(instruction.argument_0, instruction.argument_1, instruction.argument_2);
            break;
        case PlcInstructionCode::PLCINSTRUCTIONCODE_AND:
            execution_success = And(instruction.argument_0, instruction.argument_1, instruction.argument_2);
            break;
        case PlcInstructionCode::PLCINSTRUCTIONCODE_OR:
            execution_success = Or(instruction.argument_0, instruction.argument_1, instruction.argument_2);
            break;
        case PlcInstructionCode::PLCINSTRUCTIONCODE_BRANCH_NOT_IF:
            execution_success = BranchNotIf(instruction.argument_0, instruction.argument_1);
            break;
        case PlcInstructionCode::PLCINSTRUCTIONCODE_BRANCH_UNDCONDITIONAL:
            execution_success = BranchUnconditional(instruction.argument_0);
            break;
        case PlcInstructionCode::PLCINSTRUCTIONCODE_PUSH:
            execution_success = Push(instruction.argument_0);
            break;
        case PlcInstructionCode::PLCINSTRUCTIONCODE_POP:
            execution_success = Pop(instruction.argument_0);
            break;
        default:
            execution_success = false;
            break;
        }
    }

    return execution_success;
}

bool Plc::Noop(void)
{
    program_counter_++;

    return true;
}

bool Plc::ReadImmediate(const PlcInstructionArgument register_number, const PlcInstructionArgument immediate_value)
{
    bool success = false;

    if (register_number >= registers_.size())
    {
        // Invalid register, do nothing
    }
    else
    {
        registers_[register_number] = immediate_value;

        success = true;
    }

    program_counter_++;

    return success;
}

bool Plc::ReadSignal(const PlcInstructionArgument register_number, const PlcInstructionArgument input_signal)
{
    bool     success = false;
    IoSignal signal  = IoSignal::IOSIGNAL_LOW;

    if (register_number >= registers_.size())
    {
        // Invalid register, do nothing
    }
    else if (Error::ERROR_NONE == get_input_(static_cast<InputId>(input_signal), signal))
    {
        if (IoSignal::IOSIGNAL_HIGH == signal)
        {
            registers_[register_number] = 1;
        }
        else
        {
            registers_[register_number] = 0;
        }

        success = true;
    }

    program_counter_++;

    return success;
}

bool Plc::WriteSignal(const PlcInstructionArgument output_signal, const PlcInstructionArgument immediate_value)
{
    bool     success = false;
    IoSignal signal  = IoSignal::IOSIGNAL_LOW;

    if (0 != immediate_value)
    {
        signal = IoSignal::IOSIGNAL_HIGH;
    }

    if (Error::ERROR_NONE == set_output_(static_cast<OutputId>(output_signal), signal))
    {
        success = true;
    }

    program_counter_++;

    return success;
}

bool Plc::Equals(const PlcInstructionArgument output_register_number, const PlcInstructionArgument left_register_number, const PlcInstructionArgument right_register_number)
{
    bool success = false;

    if ((output_register_number >= registers_.size()) || (left_register_number >= registers_.size()) || (right_register_number >= registers_.size()))
    {
        // Invalid register(s), do nothing
    }
    else if ((registers_[left_register_number] >= 1) && (registers_[right_register_number] >= 1))
    {
        registers_[output_register_number] = 1;

        success = true;
    }
    else if ((registers_[left_register_number] == 0) && (registers_[right_register_number] == 0))
    {
        registers_[output_register_number] = 1;

        success = true;
    }
    else
    {
        registers_[output_register_number] = 0;

        success = true;
    }

    program_counter_++;

    return success;
}

bool Plc::And(const PlcInstructionArgument output_register_number, const PlcInstructionArgument left_register_number, const PlcInstructionArgument right_register_number)
{
    bool success = false;

    if ((output_register_number >= registers_.size()) || (left_register_number >= registers_.size()) || (right_register_number >= registers_.size()))
    {
        // Invalid register(s), do nothing
    }
    else if ((registers_[left_register_number] >= 1) && (registers_[right_register_number] >= 1))
    {
        registers_[output_register_number] = 1;

        success = true;
    }
    else
    {
        registers_[output_register_number] = 0;

        success = true;
    }

    program_counter_++;

    return success;
}

bool Plc::Or(const PlcInstructionArgument output_register_number, const PlcInstructionArgument left_register_number, const PlcInstructionArgument right_register_number)
{
    bool success = false;

    if ((output_register_number >= registers_.size()) || (left_register_number >= registers_.size()) || (right_register_number >= registers_.size()))
    {
        // Invalid register(s), do nothing
    }
    else if ((registers_[left_register_number] >= 1) || (registers_[right_register_number] >= 1))
    {
        registers_[output_register_number] = 1;

        success = true;
    }
    else
    {
        registers_[output_register_number] = 0;

        success = true;
    }

    program_counter_++;

    return success;
}

bool Plc::BranchNotIf(const PlcInstructionArgument register_number, const PlcInstructionArgument instruction_count)
{
    bool success = false;

    if (register_number >= registers_.size())
    {
        // Invalid register, do nothing
    }
    else if (0 == registers_[register_number])
    {
        program_counter_ += (instruction_count + 1);
        success           = true;
    }
    else
    {
        program_counter_++;
        success = true;
    }

    return success;
}

bool Plc::BranchUnconditional(const PlcInstructionArgument instruction_count)
{
    program_counter_ += (instruction_count + 1);

    return true;
}

bool Plc::Push(const PlcInstructionArgument register_number)
{
    bool success = false;

    if (register_number < registers_.size())
    {
        stack_.emplace_back(registers_[register_number]);

        success = true;
    }

    program_counter_++;

    return success;
}

bool Plc::Pop(const PlcInstructionArgument register_number)
{
    bool success = false;

    if (register_number < registers_.size())
    {
        registers_[register_number] = stack_.back();
        stack_.pop_back();

        success = true;
    }

    program_counter_++;

    return success;
}

} // namespace wayside_controller