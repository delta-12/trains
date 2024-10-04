/*****************************************************************************
* @file wayside_controller.h
*
* @brief Implements the wayside controller backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H

#include <vector>

//block information
struct Block
{
    int blocknum;
    bool fail_state;
    bool occupancy;

    bool switch_state;
    bool signal_state;
    bool crossing_state;

    std::vector<int, int> switchnum;
    int signalnum;
    int crossingnum;

    float spd_suggested;
    std::vector<int, int> auth_suggested;
};

class WaysideController
{
public:
        //primary variables
        bool switch_state, crossing_state, signal_light;
        double commanded_speed;
        int authority;

        //identifiers
        int switch_id, crossing_id, signal_id;

        //incoming from CTC
        float CTC_suggestedspd;
        std::vector<int, int> CTC_authority;

        //base level control functions
        WaysideController();
        void setSwitch(int switch_id);
        void setCrossing(int crossing_id);
        void setSignal(int signal_id);

        void getSwitch(bool);
        void getCrossing(bool);
        void getSignal(bool);

        void getTrackFailures(bool);

        //block handling functions
        void blockInit(int block_num);
        void setOccupancy(); //this information comes from TKM and is sent to CTC
        void setSpeed(int CTC_speed);
        void setAuth(int CTC_auth);








};

class PLC
{
    public:

        struct PLCpacket{
            bool currentOccupancy;
            bool nextOccupancy;
          
        }

        //plc processing functions
        void PLCget(int[]);
        void PLCset(int[]);

};







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

 #endif /*TrackController_h*/
* /