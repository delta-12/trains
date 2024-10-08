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
            virtual types::Watts GetCommandedPower(void) const                              =0;
            virtual uint16_t GetServiceBrake(void) const                                    =0;
            virtual bool GetEmergencyBrake(void) const                                      =0;
            virtual bool GetHeadLights(void) const                                          =0;
            virtual bool GetInteriorLights(void) const                                      =0;
            virtual bool GetLeftDoors(void) const                                           =0;
            virtual bool GetRightDoors(void) const                                          =0;
            virtual std::string GetStationAnnouncement(void) const                          =0;
            virtual types::Meters GetDistanceTravelled(void) const                          =0;
            virtual types::DegreesFahrenheit GetInternalTemperature(void) const             =0;

            // inputs from train model
            virtual void SetCommandedSpeedWS(types::MetersPerSecond)                        =0;
            virtual void SetCurrentVelocity(types::MetersPerSecond)                         =0;
            virtual void SetBeaconData(types::BeaconData &data)                             =0;
            virtual void SetAuthority(types::BlockId)                                       =0;
            virtual bool SetEngineFailure(bool)                                             =0;
            virtual bool SetSignalPickupFailure(bool)                                       =0;
            virtual bool SetBrakeFailure(bool)=0;
            
            // inputs from driver
            virtual void SetDriverSpeed(uint16_t)                                           =0;
            virtual void SetCommandedInternalTemperature(uint16_t)                          =0;
            virtual void SetServiceBrake(uint16_t)                                          =0;
            virtual void setEmergencyBrake(bool)                                            =0;

            // inputs from engineer
            virtual void SetKP(uint16_t)                                                    =0;
            virtual void setKI(uint16_t)                                                    =0;
    };


    class SoftwareTrainController : public TrainController
    {
        public:
                uint8_t Kp_ = 2;
                uint8_t Ki_ = 4;

                types::MetersPerSecond CommandedSpeedWS_;
                types::MetersPerSecond CurrentVelocity_;
                types::Watts CommandedPower_;

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

            std::unordered_map<types::BlockId, std::array<float, 5>> blueBlockDataMap = {
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
                {16, {50.0, 0.0, 50.0, 0.00, 0.00}}, // added 16th block
            };

            // map for block data --> 
            // key: 
            //          block number
            // values:
            //          infrastructure string

            std::unordered_map<types::BlockId, std::string> blueBlockInfrastructureMap = {
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
            virtual types::Watts GetCommandedPower(void) const;
            virtual uint16_t GetServiceBrake(void) const;
            virtual bool GetEmergencyBrake(void) const;
            virtual bool GetHeadLights(void) const;
            virtual bool GetInteriorLights(void) const;
            virtual bool GetLeftDoors(void) const;
            virtual bool GetRightDoors(void) const;
            virtual std::string GetStationAnnouncement(void) const;
            virtual types::Meters GetDistanceTravelled(void) const;
            virtual types::DegreesFahrenheit GetInternalTemperature(void) const;

            // inputs from train model
            virtual void SetCommandedSpeedWS(types::MetersPerSecond);
            virtual void SetCurrentVelocity(types::MetersPerSecond);
            virtual void SetBeaconData(types::BeaconData &data);
            virtual void SetAuthority(types::BlockId);
            virtual bool SetEngineFailure(bool);
            virtual bool SetSignalPickupFailure(bool);
            virtual bool SetBrakeFailure(bool);
            
            // inputs from driver
            virtual void SetDriverSpeed(uint16_t);
            virtual void SetCommandedInternalTemperature(uint16_t);
            virtual void SetServiceBrake(uint16_t);
            virtual void setEmergencyBrake(bool);

            // inputs from engineer
            virtual void SetKP(uint16_t);
            virtual void setKI(uint16_t);


            /////////////////////////////
            //FINISH ADDING UI FUNCTIONS//
            /////////////////////////////


            
            //TestBench Functions. These will be tied to the testbench UI
            virtual void SetCommandedSpeedTB(types::MetersPerSecond);
            virtual void SetCurrentVelocityTB(types::MetersPerSecond);
            virtual std::size_t SetBeaconDataTB(types::BeaconData &data);
            virtual types::Meters SetAuthorityTB(void);
            virtual types::Watts SetActualPowerTB(void);
            virtual bool SetEngineFailureTB(void);
            virtual bool SetSignalPickupFailureTB(void);
            virtual bool SetBrakeFailureTB(void);


            //local functions
            virtual void CalculateCommandedPower(types::MilesPerHour);
            virtual types::Watts CalculateMaxPower();
            virtual types::Meters CalculateDistanceTraveled(float);

        private:
            float IntegralSUM = 0;
    };

} // namespace train_controller

#endif // TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H