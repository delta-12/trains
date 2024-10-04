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

typedef struct BlockState BlockState;
typedef uint16_t          InputId;
typedef uint16_t          OutputId;

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

// block information
// struct Block
// {
//     int blocknum;
//     bool fail_state;
//     bool occupancy;

//     bool switch_state;
//     bool signal_state;
//     bool crossing_state;

//     std::vector<int, int> switchnum;
//     int signalnum;
//     int crossingnum;

//     float spd_suggested;
//     std::vector<int, int> auth_suggested;
// };

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
};

// class PLC
// {
//     public:
//         struct PLCpacket
//         {
//             bool currentOccupancy;
//             bool nextOccupancy;

//         }

//         // plc processing functions
//         void PLCget(int[]);
//         void PLCset(int[]);
// };

/*
        TrackController();
        void setUpController(int, std::string &, std::vector<char> &, std::vector<int> &, int[], int, int[]);
        void setSignalsInstance(CTCSignals &);
        void setSwitch(bool);
        void setSwitchAuto();
        void addBlockObj(int);
        void setTrackSA();
        void setRoute();
                int getResult();
        void setCross();
        void manSetCross(bool);
        void getFaults();
        void getOccupancies();
        void updateData();
   };
 */

} // namespace wayside_controller

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H