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
            //Getters
            virtual types::MilesPerHour GetCurrentVelocity(void) const = 0;
            virtual types::MilesPerHour GetDriverSpeed(void) const = 0;
            virtual types::MilesPerHour GetCommandedSpeedWS(void) const = 0;
            virtual types::Miles GetDistanceTravelled(void) const = 0;
            virtual types::Watts GetCommandedPower(void) const = 0;
            virtual uint16_t GetServiceBrake(void) const = 0;
            virtual uint16_t GetGrade(void) const = 0;
            virtual bool GetEmergencyBrake(void) const = 0;
            virtual bool GetHeadLights(void) const = 0;
            virtual bool GetInteriorLights(void) const = 0;
            virtual bool GetLeftDoors(void) const = 0;
            virtual bool GetRightDoors(void) const = 0;
            virtual bool GetBrakeFailure(void) const = 0;
            virtual bool GetSignalPickupFailure(void) const = 0;
            virtual bool GetEngineFailure(void) const = 0;
            virtual types::DegreesFahrenheit GetInternalTemperature(void) const = 0;
            virtual types::DegreesFahrenheit GetActInternalTemperature(void) const = 0;
            virtual types::Blocks GetAuthority(void) const = 0;  // Getter for Authority

            // Setter declarations
            virtual void SetCommandedSpeedWS(types::MilesPerHour speed) = 0;
            virtual void SetDriverSpeed(types::MilesPerHour speed) = 0;
            virtual void SetCurrentVelocity(types::MilesPerHour velocity) = 0;
            virtual void SetServiceBrake(uint16_t percentage) = 0;
            virtual void setEmergencyBrake(bool state) = 0;
            virtual void SetHeadLights(bool state) = 0;
            virtual void SetInteriorLights(bool state) = 0;
            virtual void SetLeftDoors(bool state) = 0;
            virtual void SetRightDoors(bool state) = 0;
            virtual void SetBrakeFailure(bool state) = 0;
            virtual void SetSignalPickupFailure(bool state) = 0;
            virtual void SetEngineFailure(bool state) = 0;
            virtual void SetInternalTemperature(types::DegreesFahrenheit temp) = 0;
            virtual void SetActInternalTemperature(types::DegreesFahrenheit temp) = 0;
            virtual void SetAuthority(types::Blocks authority) = 0;
            virtual void SetKP(uint16_t) = 0;
            virtual void setKI(uint16_t) = 0;
    };


    class SoftwareTrainController : public TrainController
    {
        public:

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

            public:
              // Implementations for getters
                types::MetersPerSecond GetCurrentVelocity(void) const;
                types::MetersPerSecond GetDriverSpeed(void) const;
                types::MetersPerSecond GetCommandedSpeedWS(void) const;
                types::Meters GetDistanceTravelled(void) const;
                types::Watts GetCommandedPower(void) const;
                uint16_t GetServiceBrake(void) const;
                uint16_t GetGrade(void) const;
                bool GetEmergencyBrake(void) const;
                bool GetHeadLights(void) const;
                bool GetInteriorLights(void) const;
                bool GetLeftDoors(void) const;
                bool GetRightDoors(void) const;
                bool GetBrakeFailure(void) const;
                bool GetSignalPickupFailure(void) const;
                bool GetEngineFailure(void) const;
                types::DegreesFahrenheit GetInternalTemperature(void) const;
                types::DegreesFahrenheit GetActInternalTemperature(void) const;
                types::Blocks GetAuthority(void) const;  // Getter for Authority

                // Implementations for setters
                void SetCommandedSpeedWS(types::MilesPerHour speed);
                void SetDriverSpeed(types::MilesPerHour speed);
                void SetCurrentVelocity(types::MilesPerHour velocity);
                void SetServiceBrake(uint16_t percentage);
                void setEmergencyBrake(bool state);
                void SetHeadLights(bool state);
                void SetInteriorLights(bool state);
                void SetLeftDoors(bool state);
                void SetRightDoors(bool state);
                void SetBrakeFailure(bool state);
                void SetSignalPickupFailure(bool state);
                void SetEngineFailure(bool state);
                void SetInternalTemperature(types::DegreesFahrenheit temp);
                void SetActInternalTemperature(types::DegreesFahrenheit temp);
                void SetAuthority(types::Blocks authority);
                void SetArrived(bool);
                void SetKP(uint16_t);
                void setKI(uint16_t);


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
            virtual void CalculateCommandedPower();
            virtual void UpdateDistanceTravelled(long interval);
            void Update();

        private:
            float IntegralSUM = 0;
            float Kp_ = 4;
            float Ki_ = 2;


            types::Watts maxPower = 120000;
            types::MilesPerHour CommandedSpeedWS_ = 0;
            types::MilesPerHour DriverSpeed_;
            uint16_t SBPercentage_;
            types::MilesPerHour CurrentVelocity_ = 0;
            types::Watts CommandedPower_ = 0;
            types::Blocks Authority_ = 0;
            bool Ebrake = 0;
            bool HLights = 0;
            bool ILights = 0;
            bool LDoor = 0;
            bool RDoor = 0;
            bool BFail = 0;
            bool SigPFail = 0;
            bool EFail = 0;
            types::DegreesFahrenheit Temp = 68;
            types::DegreesFahrenheit ActualTemp_ = 0;
            types::Meters DistanceTravelled_ = 0;
            bool Arrived = 0;
    };

} // namespace train_controller

#endif // TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H