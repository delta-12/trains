#ifndef PLC_H
#define PLC_H

#include <deque>
#include <functional>

#include "types.h"
#include "wayside_controller.h"

namespace wayside_controller
{

using PlcInstructionArgument = uint32_t;
using PlcInstruction         = struct PlcInstruction;

static const size_t kPlcRegisters = 2;

enum class PlcInstructionCode
{
    PLCINSTRUCTIONCODE_NOOP,
    PLCINSTRUCTIONCODE_READ_IMMEDIATE,
    PLCINSTRUCTIONCODE_READ_SIGNAL,
    PLCINSTRUCTIONCODE_WRITE_SIGNAL,
    PLCINSTRUCTIONCODE_EQUALS,
    PLCINSTRUCTIONCODE_AND,
    PLCINSTRUCTIONCODE_OR,
    PLCINSTRUCTIONCODE_BRANCH_NOT_IF,
    PLCINSTRUCTIONCODE_BRANCH_UNDCONDITIONAL,
    PLCINSTRUCTIONCODE_PUSH,
    PLCINSTRUCTIONCODE_POP
};

struct PlcInstruction
{
    public:
        PlcInstruction(void);
        PlcInstruction(const PlcInstructionCode instruction_code, const PlcInstructionArgument argument_0, const PlcInstructionArgument argument_1,
                       const PlcInstructionArgument argument_2);
        PlcInstructionCode instruction_code;
        PlcInstructionArgument argument_0;
        PlcInstructionArgument argument_1;
        PlcInstructionArgument argument_2;
};

class Plc
{
    public:
        Plc(const std::function<Error(const InputId input, IoSignal &signal)> &get_input,
            const std::function<Error(const OutputId output, const IoSignal signal)> &set_output);
        Plc(const std::function<Error(const InputId input, IoSignal &signal)> &get_input,
            const std::function<Error(const OutputId output, const IoSignal signal)> &set_output,
            const std::vector<PlcInstruction> &instructions);
        void SetInstructions(const std::vector<PlcInstruction> &instructions);
        uint32_t GetProgramCounter(void) const;
        PlcInstruction GetInstruction(void) const;
        bool Run(void);

    private:
        bool Noop(void);
        bool ReadImmediate(const PlcInstructionArgument register_number, const PlcInstructionArgument immediate_value);
        bool ReadSignal(const PlcInstructionArgument register_number, const PlcInstructionArgument input_signal);
        bool WriteSignal(const PlcInstructionArgument output_signal, const PlcInstructionArgument immediate_value);
        bool Equals(const PlcInstructionArgument output_register_number, const PlcInstructionArgument left_register_number, const PlcInstructionArgument right_register_number);
        bool And(const PlcInstructionArgument output_register_number, const PlcInstructionArgument left_register_number, const PlcInstructionArgument right_register_number);
        bool Or(const PlcInstructionArgument output_register_number, const PlcInstructionArgument left_register_number, const PlcInstructionArgument right_register_number);
        bool BranchNotIf(const PlcInstructionArgument register_number, const PlcInstructionArgument instruction_count);
        bool BranchUnconditional(const PlcInstructionArgument instruction_count);
        bool Push(const PlcInstructionArgument register_number);
        bool Pop(const PlcInstructionArgument register_number);

        std::function<Error(const InputId input, IoSignal &signal)> get_input_;
        std::function<Error(const OutputId output, const IoSignal signal)> set_output_;
        std::vector<PlcInstruction> instructions_;
        uint32_t program_counter_                                    = 0;
        std::array<PlcInstructionArgument, kPlcRegisters> registers_ = {0};
        std::deque<PlcInstructionArgument> stack_;
};

} // namespace wayside_controller

#endif