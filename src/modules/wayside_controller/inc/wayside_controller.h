/*****************************************************************************
* @file wayside_controller.h
*
* @brief Implements the wayside controller backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H

#include <memory>
#include <unordered_map>
#include <vector>

#include "types.h"

namespace wayside_controller
{

typedef struct BlockState       BlockState;
typedef struct BlockInput       BlockInput;
typedef struct TrackCircuitData TrackCircuitData;
typedef struct PlcInstrction    PlcInstrction;
typedef uint16_t                InputId;
typedef uint16_t                OutputId;
typedef uint32_t                PlcInstructionArgument;

typedef enum
{
    PLCINSTRUCTIONCODE_READ,
    PLCINSTRUCTIONCODE_WRITE,
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

struct BlockInput
{
    public:
        BlockInput(const types::BlockId block, InputId input, const bool state);
        types::BlockId block;
        InputId input;
        bool state;
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
        PlcInstruction(const PlcInstructionCode instruction_code, const PlcInstructionArgument argument_0, const PlcInstructionArgument argument_1, const PlcInstructionArgument argument_2);
        PlcInstructionCode instruction_code;
        PlcInstructionArgument argument_0;
        PlcInstructionArgument argument_1;
        PlcInstructionArgument argument_2;
};

class WaysideController
{
    public:
        WaysideController(std::shared_ptr<void(std::unordered_map<InputId, bool> &inputs)> get_inputs, std::shared_ptr<types::Error(const OutputId output, const bool state)> set_output);
        WaysideController(std::shared_ptr<void(std::unordered_map<InputId, bool> &inputs)> get_inputs, std::shared_ptr<types::Error(const OutputId output, const bool state)> set_output, const std::vector<BlockInput> &block_input_map);
        void SetBlockMap(const std::vector<BlockInput> &block_input_map);
        void SetOutput(const OutputId output, const bool state); // check outputs corresponding to switches to verify safety
        void GetInput(const InputId input, bool &state);
        void ScanInputs(void);
        types::Error GetCommandedSpeedAndAuthority(TrackCircuitData &track_circuit_data); // check for safe speed and authority
        void SetManualMode(const types::BlockId block, const bool manual_mode);
        void SetSwitch(const types::BlockId, const bool switch_state); // can be used in both auto and maintenance mode?
        std::vector<BlockState> GetBlockStates(void);

    private:
        std::shared_ptr<void(std::unordered_map<InputId, bool> &inputs)> get_inputs_;
        std::shared_ptr<types::Error(const OutputId output, const bool state)> set_output_;
        std::vector<BlockInput> block_input_map_;
        std::unordered_map<InputId, bool> inputs_;
};

class Plc
{
    public:
        Plc(std::shared_ptr<WaysideController> wayside_controller);
        Plc(std::shared_ptr<WaysideController> wayside_controller, const std::vector<PlcInstruction> &instructions);
        void SetInstructions(const std::vector<PlcInstruction> &instructions);
        void Update(void);

    private:
        uint32_t register_0_;
        uint32_t register_1_;
        uint32_t register_2_;
};

} // namespace wayside_controller

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H