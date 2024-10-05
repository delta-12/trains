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
        BlockState(const types::TrackId track, const types::BlockId block, const bool occupied, const bool track_failure);
        types::TrackId track;
        types::BlockId block;
        bool occupied;
        bool track_failure;
};

struct TrackCircuitData
{
    public:
        TrackCircuitData(const types::TrackId track, const types::BlockId block, const types::MetersPerSecond speed, const types::Meters authority);
        types::TrackId track;
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
        WaysideController(std::shared_ptr<void(std::unordered_map<InputId, bool> &inputs)> get_inputs, std::shared_ptr<void(std::unordered_map<OutputId, bool> &inputs)> set_outputs);
        void SetOutput(const OutputId output, const bool state); // check outputs corresponding to switches to verify safety
        void GetInput(const InputId input, bool &state);
        types::Error GetCommandedSpeedAndAuthority(TrackCircuitData &track_circuit_data); // check for safe speed and authority
        void SetManualMode(const types::TrackId track, const types::BlockId block, const bool manual_mode);
        void SetSwitch(const types::TrackId track, const types::BlockId, const bool switch_state); // can be used in both auto and maintenance mode?
        std::vector<BlockState> GetBlockStates(const types::TrackId track);

    private:
        std::shared_ptr<void(std::unordered_map<InputId, bool> &inputs)> get_inputs_;
        std::shared_ptr<void(std::unordered_map<OutputId, bool> &inputs)> set_outputs_;
        std::unordered_map<InputId, bool> inputs_;
        std::unordered_map<OutputId, bool> outputs_;
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