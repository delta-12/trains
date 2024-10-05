/*****************************************************************************
* @file wayside_controller.h
*
* @brief Implements the wayside controller backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

#include "types.h"

#define WAYSIDE_CONTROLLER_PHYSICAL_INPUT_COUNT       75
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

typedef struct BlockState       BlockState;
typedef struct BlockInputs      BlockInputs;
typedef struct TrackCircuitData TrackCircuitData;
typedef struct PlcInstruction   PlcInstruction;
typedef uint16_t                InputId;
typedef uint16_t                OutputId;
typedef uint32_t                PlcInstructionArgument;

typedef enum
{
    ERROR_NONE,
    ERROR_INVALID_INPUT,
    ERROR_INVALID_OUTPUT
} Error;

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

struct BlockState
{
    public:
        BlockState(const types::BlockId block, const bool occupied, const bool track_failure);
        types::BlockId block;
        bool occupied;
        bool track_failure;
};

struct BlockInputs
{
    public:
        BlockInputs(void);
        BlockInputs(const types::BlockId block, const InputId track_circuit_input, const InputId switch_input, const bool has_switch, const bool maintenance_mode);
        types::BlockId block;
        InputId track_circuit_input;
        InputId switch_input;
        bool has_switch;
        bool maintenance_mode;
};

struct TrackCircuitData
{
    public:
        TrackCircuitData(const types::BlockId block, const types::MetersPerSecond speed, const types::Meters authority);
        types::BlockId block;
        types::MetersPerSecond speed;
        types::Meters authority;
};

struct PlcInstruction
{
    public:
        PlcInstruction(void);
        PlcInstruction(const PlcInstructionCode instruction_code, const PlcInstructionArgument argument_0, const PlcInstructionArgument argument_1, const PlcInstructionArgument argument_2);
        PlcInstructionCode instruction_code;
        PlcInstructionArgument argument_0;
        PlcInstructionArgument argument_1;
        PlcInstructionArgument argument_2;
};

class WaysideController
{
    public:
        WaysideController(std::shared_ptr<void(std::array<bool, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT> &inputs)> get_inputs, std::shared_ptr<Error(const OutputId output, const bool state)> set_output);
        WaysideController(std::shared_ptr<void(std::array<bool, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT> &inputs)> get_inputs, std::shared_ptr<Error(const OutputId output, const bool state)> set_output, const std::vector<BlockInputs> &block_inputs_map);
        Error SetBlockMap(const std::vector<BlockInputs> &block_inputs_map);
        Error SetOutput(const OutputId output, const bool state); // check outputs corresponding to switches to verify safety
        Error GetInput(const InputId input, bool &state);
        void ScanInputs(void);
        types::Error GetCommandedSpeedAndAuthority(TrackCircuitData &track_circuit_data); // check for safe speed and authority
        types::Error SetMaintenanceMode(const types::BlockId block, const bool maintenance_mode);
        types::Error SetSwitch(const types::BlockId block, const bool switch_state); // can be used in both auto and maintenance mode?
        std::vector<BlockState> GetBlockStates(void);

    private:
        bool IsTrackCircuitInputValid(const InputId input) const;
        bool IsSwitchInputValid(const InputId input) const;

        std::shared_ptr<void(std::array<bool, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT> &inputs)> get_inputs_;
        std::shared_ptr<Error(const OutputId output, const bool state)> set_output_;
        std::unordered_map<types::BlockId, BlockInputs> block_inputs_map_;
        std::array<bool, WAYSIDE_CONTROLLER_TOTAL_INPUT_COUNT> inputs_;
};

class Plc
{
    public:
        Plc(void);
        Plc(const std::vector<PlcInstruction> &instructions);
        void SetInstructions(const std::vector<PlcInstruction> &instructions);
        uint32_t GetProgramCounter(void) const;
        PlcInstruction GetInstruction(void) const;
        bool Run(WaysideController &wayside_controller);

    private:
        bool ReadSignal(WaysideController &wayside_controller, const PlcInstructionArgument register_number, const PlcInstructionArgument input);
        bool WriteSignal(WaysideController &wayside_controller, const PlcInstructionArgument register_number, const PlcInstructionArgument output);
        std::vector<PlcInstruction> instructions_;
        uint32_t program_counter_;
        std::array<PlcInstructionArgument, WAYSIDE_CONTROLLER_PLC_REGISTER_COUNT> registers_;
};

} // namespace wayside_controller

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H