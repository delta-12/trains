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
            //Functions that will output from track controller
            virtual types::Watts GetCommandedPower(float speed)                             =0;
            virtual types::Watts GetBrake(float percent)                                    =0;
            virtual void GetEmergencyBrake(void)                                            =0;
            virtual bool GetHeadLights(void)                                                =0;
            virtual bool GetInteriorLights(void)                                            =0;
            virtual bool GetLeftDoors(void)                                                 =0;
            virtual bool GetRightDoors(void)                                                =0;
            virtual std::string GetStationAnnouncement(void) const                          =0;
            virtual types::Meters GetDistanceTravelled(void) const                          =0;
            virtual void GetInternalTemperature(types::DegreesFahrenheit temp)              =0;
            virtual std::uint16_t GetInternalTemperature(void) const                        =0;

            // inputs from train model
            virtual types::MetersPerSecond SetCommandedSpeed(void) const                    =0;
            virtual types::MetersPerSecond SetActualSpeed(void) const                       =0;
            virtual std::size_t SetBeaconData(types::BeaconData &data) const                =0;
            virtual types::Meters SetAuthority(void) const                                  =0;
            virtual types::Watts SetActualPower(void) const                                 =0;
            virtual bool SetEngineFailure(void) const                                       =0;
            virtual bool SetSignalPickupFailure(void) const                                 =0;
            virtual bool SetBrakeFailure(void) const;
            
            // inputs from driver
            virtual void SetDriverSpeed(uint16_t)                                           =0;
            virtual void SetCommandedInternalTemperature(uint16_t)                          =0;
            virtual void SetServiceBrake(uint16_t)                                          =0;
            virtual void setEmergencyBrake(bool)                                            =0;

            // inputs from engineer
            virtual void SetKP(uint16_t)                                                    =0;
            virtual void setKI(uint16_t)                                                    =0;
    };


    class SoftwareTrainController : public TrainController {
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

            //Functions that will output from track controller
            virtual types::Watts GetCommandedPower(float speed);
            virtual types::Watts GetBrake(float percent);
            virtual void GetEmergencyBrake(void);
            virtual bool GetHeadLights(void);
            virtual bool GetInteriorLights(void);
            virtual bool GetLeftDoors(void);
            virtual bool GetRightDoors(void);
            virtual std::string GetStationAnnouncement(void) const;
            virtual types::Meters GetDistanceTravelled(void) const;
            virtual void GetInternalTemperature(types::DegreesFahrenheit temp);
            virtual std::uint16_t GetInternalTemperature(void) const;

            // inputs from train model
            virtual types::MetersPerSecond SetCommandedSpeed(void) const;
            virtual types::MetersPerSecond SetActualSpeed(void) const;
            virtual std::size_t SetBeaconData(types::BeaconData &data) const;
            virtual types::Meters SetAuthority(void) const;
            virtual types::Watts SetActualPower(void) const;
            virtual bool SetEngineFailure(void) const;
            virtual bool SetSignalPickupFailure(void) const;
            virtual bool SetBrakeFailure(void) const;
            
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