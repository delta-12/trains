/*****************************************************************************
* @file wayside_controller.h
*
* @brief Implements the wayside controller backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H

#include <array>
#include <functional>
#include <unordered_map>
#include <vector>

#include "graph.h"
#include "types.h"

namespace wayside_controller
{

using InputId                =  uint16_t;
using OutputId               =  uint16_t;
using WaysideBlock           =  struct WaysideBlock;
using BlockConnection        =  struct BlockConnection;
using PlcInstructionArgument =  uint32_t;
using PlcInstruction         =  struct PlcInstruction;

static const size_t kPhysicalInputs         = 78;
static const size_t kVirtualInputs          = 7;
static const size_t kTotalInputs            = kPhysicalInputs + kVirtualInputs;
static const size_t kTrafficLightsPerSwitch = 2;
static const size_t kSwitchOutputs          = 7;
static const size_t kTrafficLightOutputs    = kSwitchOutputs * kTrafficLightsPerSwitch;
static const size_t kCrossingOutputs        = 2;
static const size_t kTotalOutputs           = kSwitchOutputs + kTrafficLightOutputs + kCrossingOutputs;
static const size_t kPlcRegisters           = 3;

enum class Error
{
    ERROR_NONE,
    ERROR_INVALID_BLOCK,
    ERROR_DUPLICATE_BLOCK,
    ERROR_INVALID_INPUT,
    ERROR_DUPLICATE_INPUT,
    ERROR_INVALID_OUTPUT
};

enum class IoSignal
{
    IOSIGNAL_LOW,
    IOSIGNAL_HIGH
};

enum class PlcInstructionCode
{
    PLCINSTRUCTIONCODE_NOOP,
    PLCINSTRUCTIONCODE_READ_IMMEDIATE,
    PLCINSTRUCTIONCODE_READ_SIGNAL,
    PLCINSTRUCTIONCODE_WRITE_SIGNAL,
    PLCINSTRUCTIONCODE_EQUALS,
    PLCINSTRUCTIONCODE_OR,
    PLCINSTRUCTIONCODE_BRANCH_IF,
    PLCINSTRUCTIONCODE_BRANCH_UNDCONDITIONAL
};

/*
enum class TrackFailures
{
    TRACK_CIRCUIT_FAILURE,
    POWER_FAILURE,
    BROKEN_RAIL
}; */


// TODO NNF-144 add block speed limit
struct WaysideBlock
{
    public:
        WaysideBlock(void);
        WaysideBlock(const types::BlockId block, const types::BlockId primary_connection, const types::BlockId switch_connection, const types::BlockDirection direction,
                     const InputId track_circuit_input, const InputId switch_input, const bool has_switch, const bool maintenance_mode, const IoSignal occupancy_signal);
        types::BlockId block;
        types::BlockId primary_connection;
        types::BlockId switch_connection;
        types::BlockDirection direction;
        InputId track_circuit_input;
        InputId switch_input;
        bool has_switch;
        bool maintenance_mode;
        IoSignal occupancy_signal;
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

class WaysideController
{
    public:
        //default constructor
        WaysideController(void);
        
        explicit WaysideController(const std::function<Error(const InputId input, IoSignal &signal)> &get_input);
        WaysideController(const std::function<Error(const InputId input, IoSignal &signal)> &get_input, const std::vector<WaysideBlock> &blocks);
        Error Configure(const std::vector<WaysideBlock> &blocks);
        Error GetCommandedSpeedAndAuthority(types::TrackCircuitData &track_circuit_data);
        Error SetMaintenanceMode(const types::BlockId block, const bool maintenance_mode);
        Error SetSwitch(const types::BlockId block, const bool switch_state);
        Error GetBlockStates(std::vector<types::BlockState> &block_states);

        //getters
        std::unordered_map<types::BlockId, WaysideBlock> GetBlockConfiguration(void);
        Graph<types::BlockId, uint8_t> GetBlockLayout(void);

        int GetCommandedAuthority(types::TrackCircuitData &track_circuit_data);
        double GetCommandedSpeed(types::TrackCircuitData &track_circuit_data);


    

    private:
        static bool IsTrackCircuitInputValid(const InputId input);
        static bool IsSwitchInputValid(const InputId input);

        std::function<Error(const InputId input, IoSignal &signal)> get_input_;
        std::unordered_map<types::BlockId, WaysideBlock> block_configuration_;
        Graph<types::BlockId, uint8_t> block_layout_;
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
        bool ReadSignal(const PlcInstructionArgument register_number, const PlcInstructionArgument input);
        bool WriteSignal(const PlcInstructionArgument register_number, const PlcInstructionArgument output);

        std::function<Error(const InputId input, IoSignal &signal)> get_input_;
        std::function<Error(const OutputId output, const IoSignal signal)> set_output_;
        std::vector<PlcInstruction> instructions_;
        uint32_t program_counter_ = 0;
        std::array<PlcInstructionArgument, kPlcRegisters> registers_; // TODO NNF-104 verify registers are initialized to 0
};

} // namespace wayside_controller

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H