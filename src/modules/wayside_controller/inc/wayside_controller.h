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

using SignalId        = uint16_t;
using InputId         = SignalId;
using OutputId        = SignalId;
using WaysideBlock    = struct WaysideBlock;
using BlockConnection = struct BlockConnection;

static const size_t kPhysicalInputs         = 78;
static const size_t kVirtualInputs          = 7;
static const size_t kTotalInputs            = kPhysicalInputs + kVirtualInputs;
static const size_t kTrafficLightsPerSwitch = 2;
static const size_t kSwitchOutputs          = 7;
static const size_t kTrafficLightOutputs    = kSwitchOutputs * kTrafficLightsPerSwitch;
static const size_t kCrossingOutputs        = 2;
static const size_t kTotalOutputs           = kSwitchOutputs + kTrafficLightOutputs + kCrossingOutputs;

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

// TODO NNF-144 add block speed limit
struct WaysideBlock
{
    public:
        WaysideBlock(void);
        WaysideBlock(const types::BlockId block, const types::BlockId next_block, const types::BlockId primary_connection, const types::BlockId secondary_connection,
                     const types::BlockDirection direction, const InputId track_circuit_input, const InputId switch_input, const OutputId switch_output, const bool has_switch,
                     const bool maintenance_mode, const IoSignal occupancy_signal);
        types::BlockId block;
        types::BlockId next_block;
        types::BlockId primary_connection;
        types::BlockId secondary_connection;
        types::BlockDirection direction;
        InputId track_circuit_input;
        InputId switch_input;
        OutputId switch_output;
        bool has_switch;
        bool maintenance_mode;
        IoSignal occupancy_signal;
};

class WaysideController
{
    public:
        explicit WaysideController(const std::function<Error(const InputId input, IoSignal &signal)> &get_input);
        WaysideController(const std::function<Error(const InputId input, IoSignal &signal)> &get_input, const std::vector<WaysideBlock> &blocks);
        Error Configure(const std::vector<WaysideBlock> &blocks);
        Error GetCommandedSpeedAndAuthority(types::TrackCircuitData &track_circuit_data);
        Error SetMaintenanceMode(const types::BlockId block, const bool maintenance_mode);
        Error SetSwitch(const types::BlockId block, const bool switch_state);
        Error GetBlockStates(std::vector<types::BlockState> &block_states);
        Error UpdateSwitchPosition(const OutputId switch_output, const IoSignal signal);

    private:
        static bool IsTrackCircuitInputValid(const InputId input);
        static bool IsSwitchInputValid(const InputId input);

        std::function<Error(const InputId input, IoSignal &signal)> get_input_;
        std::unordered_map<types::BlockId, WaysideBlock> block_configuration_;
        Graph<types::BlockId, uint8_t> block_layout_;
};

} // namespace wayside_controller

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H