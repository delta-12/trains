/*****************************************************************************
* @file train_controller.h
*
* @brief Implements the train controller backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H
#define TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H


#include "types.h"
#include <array>
#include <string>
#include <cstdint>
#include <unordered_map>

namespace train_controller
{

class TrainController
{
    public:
        uint8_t Kp = 2;
        uint8_t Ki = 4;

        /*
        *
        *** BLUE LINE MAP IMPLEMENTATION ***
        *
        */ 

        // map for block data --> 
        // key: 
        //          block number
        // values:
        //          index 0: block length           (m)
        //          index 1: block grade            (%)
        //          index 2: speed limit            (Km/Hr)
        //          index 3: elevation              (m)
        //          index 4: cumulative elevation   (m)

        std::unordered_map<int, std::array<float, 5>> blockDataMap = {
            {1, {50.0, 0.0, 50.0, 0.00, 0.00}},
            {2, {50.0, 0.0, 50.0, 0.00, 0.00}},
            {3, {50.0, 0.0, 50.0, 0.00, 0.00}},
            {4, {50.0, 0.0, 50.0, 0.00, 0.00}},
            {5, {50.0, 0.0, 50.0, 0.00, 0.00}},
            {6, {50.0, 0.0, 50.0, 0.00, 0.00}},
            {7, {50.0, 0.0, 50.0, 0.00, 0.00}},
            {8, {50.0, 0.0, 50.0, 0.00, 0.00}},
            {9, {50.0, 0.0, 50.0, 0.00, 0.00}},
            {10, {50.0, 0.0, 50.0, 0.00, 0.00}},
            {11, {50.0, 0.0, 50.0, 0.00, 0.00}},
            {12, {50.0, 0.0, 50.0, 0.00, 0.00}},
            {13, {50.0, 0.0, 50.0, 0.00, 0.00}},
            {14, {50.0, 0.0, 50.0, 0.00, 0.00}},
            {15, {50.0, 0.0, 50.0, 0.00, 0.00}},
        };

        // map for block data --> 
        // key: 
        //          block number
        // values:
        //          infrastructure string

        std::unordered_map<int, std::string> blockIfrastructureMap = {
            {3, "RAILWAY CROSSING"},
            {5, "Switch ( 5 to 6) or (5 to 11)"},
            {6, "Switch ( 5 to 6); Light"},
            {9, "Transponder"},
            {10, "Station B"},
            {11, "Switch (5 to 11); Light"},
            {14, "Transponder"},
            {15, "Station C"},
        };


        virtual types::Watts CalculateCommandedPower(uint8_t speed);
        virtual types::Watts GetBrake(uint8_t percent); // ask if service brake is one time application and goes back to 0 or if it is continuous until set back to 0
        virtual void EmergencyBrake(void);
        //Functions that will output from track controller
        virtual bool ToggleHeadLights(void);
        virtual bool ToggleInteriorLights(void);
        virtual bool ToggleLeftDoors(void);
        virtual bool ToggleRightDoors(void);
        virtual std::string GetStationAnnouncement(void) const;
        virtual types::Meters GetDistanceTravelled(void) const;
        virtual void SetInternalTemperature(types::DegreesFahrenheit temp);
        virtual std::uint16_t GetInternalTemperature(void) const;

        // inputs from train model
        // virtual types::MetersPerSecond GetCommandedSpeed(void) const;
        // virtual types::MetersPerSecond GetActualSpeed(void) const;
        // virtual std::size_t GetBeaconData(types::BeaconData &data) const;
        // virtual types::Meters GetAuthority(void) const;
        // virtual types::Watts GetActualPower(void) const;
        // virtual bool GetEngineFailure(void) const;
        // virtual bool GetSignalPickupFailure(void) const;
        // virtual bool GetBrakeFailure(void) const;
        
        // inputs from driver
        virtual void SetDriverSpeed(uint16_t);
        virtual void SetCommandedInternalTemperature(uint16_t);
        virtual void SetServiceBrake(uint16_t);
        virtual void setEmergencyBrake(bool);

        // inputs from engineer
        virtual void SetKP(uint16_t);
        virtual void setKI(uint16_t);



        //TestBench Functions. These will be tied to the testbench UI
        virtual types::MetersPerSecond SetCommandedSpeedTB(void) const;
        virtual types::MetersPerSecond SetActualSpeedTB(void) const;
        virtual std::size_t SetBeaconDataTB(types::BeaconData &data) const;
        virtual types::Meters SetAuthorityTB(void) const;
        virtual types::Watts SetActualPowerTB(void) const;
        virtual bool SetEngineFailureTB(void) const;
        virtual bool SetSignalPickupFailureTB(void) const;
        virtual bool SetBrakeFailureTB(void) const;


};

} // namespace train_controller

#endif // TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H