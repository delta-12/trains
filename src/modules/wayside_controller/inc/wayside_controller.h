/*****************************************************************************
* @file wayside_controller.h
*
* @brief Implements the wayside controller backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_H

#include <vector>

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

        //base level functions
        WaysideController();
        void setSwitch(bool);
        void setCrossing(bool);
        void setSignal(bool);

        void getSwitch(bool);
        void getCrossing(bool);
        void getSignal(bool);

        void getTrackFailures(bool);



        //block information
        struct Block
        {
            bool occupancy;
            bool failure_state;
            //int block number;

        }


};

class PLC
{
    public:

        /*struct PLCpacket{


           }*/

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