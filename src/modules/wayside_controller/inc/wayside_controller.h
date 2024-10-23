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

#define WAYSIDE_CONTROLLER_PHYSICAL_INPUT_COUNT       78
#define WAYSIDE_CONTROLLER_VIRTUAL_INPUT_COUNT        7
#define WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT          (WAYSIDE_CONTROLLER_PHYSICAL_INPUT_COUNT + WAYSIDE_CONTROLLER_VIRTUAL_INPUT_COUNT)
#define WAYSIDE_CONTROLLER_TRAFFIC_LIGHTS_PER_SWITCH  3
#define WAYSIDE_CONTROLLER_SWITCH_OUTPUT_COUNT        7
#define WAYSIDE_CONTROLLER_TRAFFIC_LIGHT_OUTPUT_COUNT (WAYSIDE_CONTROLLER_SWITCH_OUTPUT_COUNT * WAYSIDE_CONTROLLER_TRAFFIC_LIGHTS_PER_SWITCH)
#define WAYSIDE_CONTROLLER_CROSSING_OUTPUT_COUNT      2
#define WAYSIDE_CONTROLLER_TOTAL_OUTPUTS              (WAYSIDE_CONTROLLER_SWITCH_OUTPUT_COUNT + WAYSIDE_CONTROLLER_TRAFFIC_LIGHT_OUTPUT_COUNT + WAYSIDE_CONTROLLER_CROSSING_OUTPUT_COUNT)
#define WAYSIDE_CONTROLLER_PLC_REGISTER_COUNT         3

namespace wayside_controller
{

typedef uint16_t               InputId;
typedef uint16_t               OutputId;
typedef struct WaysideBlock    WaysideBlock;
typedef struct BlockConnection BlockConnection;
typedef uint32_t               PlcInstructionArgument;
typedef struct PlcInstruction  PlcInstruction;

typedef enum
{
    ERROR_NONE,
    ERROR_INVALID_BLOCK,
    ERROR_DUPLICATE_BLOCK,
    ERROR_INVALID_INPUT,
    ERROR_DUPLICATE_INPUT,
    ERROR_INVALID_OUTPUT,
    ERROR_DUPLICATE_OUTPUT,
} Error;

typedef enum
{
    IOSIGNAL_LOW,
    IOSIGNAL_HIGH
} IoSignal;

typedef enum
{
    PLCINSTRUCTIONCODE_NOOP,
    PLCINSTRUCTIONCODE_READ_IMMEDIATE,
    PLCINSTRUCTIONCODE_READ_SIGNAL,
    PLCINSTRUCTIONCODE_WRITE_SIGNAL,
    PLCINSTRUCTIONCODE_EQUALS,
    PLCINSTRUCTIONCODE_OR,
    PLCINSTRUCTIONCODE_BRANCH_IF,
    PLCINSTRUCTIONCODE_BRANCH_UNDCONDITIONAL
} PlcInstructionCode;

// TODO NNF-144 add block speed limit
struct WaysideBlock
{
    public:
        WaysideBlock(void);
        WaysideBlock(const types::BlockId block, const types::BlockId primary_connection, const types::BlockId switch_connection, const types::BlockDirection direction,
                     const InputId track_circuit_input, const InputId switch_input, const bool has_switch, const bool maintenance_mode, const bool occupied);
        types::BlockId block;
        types::BlockId primary_connection;
        types::BlockId switch_connection;
        types::BlockDirection direction;
        InputId track_circuit_input;
        InputId switch_input;
        bool has_switch;
        bool maintenance_mode;
        bool occupied;
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
        WaysideController(std::function<Error(const InputId input, IoSignal &signal)> get_input);
        WaysideController(std::function<Error(const InputId input, IoSignal &signal)> get_input, const std::vector<WaysideBlock> &blocks);
        Error Configure(const std::vector<WaysideBlock> &blocks);
        Error GetCommandedSpeedAndAuthority(types::TrackCircuitData &track_circuit_data);
        Error SetMaintenanceMode(const types::BlockId block, const bool maintenance_mode);
        Error SetSwitch(const types::BlockId block, const bool switch_state);
        std::vector<types::BlockState> GetBlockStates(void);

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
        Plc(std::function<Error(const InputId input, IoSignal &signal)> get_input, std::function<Error(const OutputId output, const IoSignal signal)> set_output);
        Plc(std::function<Error(const InputId input, IoSignal &signal)> get_input, std::function<Error(const OutputId output, const IoSignal signal)> set_output, const std::vector<PlcInstruction> &instructions);
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
        uint32_t program_counter_;
        std::array<PlcInstructionArgument, WAYSIDE_CONTROLLER_PLC_REGISTER_COUNT> registers_;
};

} // namespace wayside_controller

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H