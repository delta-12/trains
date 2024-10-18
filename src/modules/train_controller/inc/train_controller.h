/*****************************************************************************
* @file train_controller.h
*
* @brief Implements the train controller backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H
#define TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H


#define TRAIN_CONTROLLER_DEFAULT_KP           (4)
#define TRAIN_CONTROLLER_DEFAULT_KI           (2)
#define TRAIN_CONTROLLER_MAXIMUM_ENGINE_POWER (120000)
#define TRAIN_SPEED_LIMIT                     (120000)
#define DEFAULT_BLOCK_SPEED_LIMIT             (50)
#define DEFAULT_BLOCK_GRADE                   (0)
#define DEFAULT_TRAIN_TEMPERATURE             (68)
#define DEFAULT_DELTA_TIME                    (1)


#include <array>
#include <string>
#include <cstdint>
#include <unordered_map>

#include "types.h"
#include "convert.h"

namespace train_controller
{

class TrainController
{
    public:
        //Getters
        virtual types::MetersPerSecond GetCurrentSpeed(void) const                   = 0;
        virtual types::MilesPerHour GetDriverSpeed(void) const                       = 0;
        virtual types::MetersPerSecond GetCommandedSpeed(void) const                 = 0;
        virtual types::Miles GetDistanceTravelled(void) const                        = 0;
        virtual types::Watts GetCommandedPower(void) const                           = 0;
        virtual double GetServiceBrake(void) const                                   = 0;
        virtual double GetGrade(void) const                                          = 0;
        virtual bool GetEmergencyBrake(void) const                                   = 0;
        virtual bool GetHeadLights(void) const                                       = 0;
        virtual bool GetInteriorLights(void) const                                   = 0;
        virtual bool GetLeftDoors(void) const                                        = 0;
        virtual bool GetRightDoors(void) const                                       = 0;
        virtual bool GetBrakeFailure(void) const                                     = 0;
        virtual bool GetSignalPickupFailure(void) const                              = 0;
        virtual bool GetEngineFailure(void) const                                    = 0;
        virtual types::DegreesFahrenheit GetCommandedInternalTemperature(void) const = 0;
        virtual types::DegreesFahrenheit GetActualInternalTemperature(void) const    = 0;
        virtual types::Meters GetAuthority(void) const                               = 0;

        // Setter declarations
        virtual void SetCommandedSpeed(const types::MetersPerSecond speed)                       = 0;
        virtual void SetDriverSpeed(const types::MilesPerHour speed)                             = 0;
        virtual void SetCurrentSpeed(const types::MetersPerSecond speed)                         = 0;
        virtual void SetServiceBrake(const double percentage)                                    = 0;
        virtual void SetEmergencyBrake(const bool state)                                         = 0;
        virtual void SetHeadLights(const bool state)                                             = 0;
        virtual void SetInteriorLights(const bool state)                                         = 0;
        virtual void SetLeftDoors(const bool state)                                              = 0;
        virtual void SetRightDoors(const bool state)                                             = 0;
        virtual void SetBrakeFailure(const bool state)                                           = 0;
        virtual void SetSignalPickupFailure(const bool state)                                    = 0;
        virtual void SetEngineFailure(const bool state)                                          = 0;
        virtual void SetCommandedInternalTemperature(const types::DegreesFahrenheit temperature) = 0;
        virtual void SetActualInternalTemperature(const types::DegreesFahrenheit temperature)    = 0;
        virtual void SetAuthority(const types::Meters authority)                                 = 0;
        virtual void SetKP(const uint16_t kp)                                                    = 0;
        virtual void SetKI(const uint16_t ki)                                                    = 0;
};


class SoftwareTrainController : public TrainController
{
    public:
        // Constructor
        SoftwareTrainController();

        // Implementations for getters
        types::MetersPerSecond GetCurrentSpeed(void) const;
        types::MilesPerHour GetDriverSpeed(void) const;
        types::MetersPerSecond GetCommandedSpeed(void) const;
        types::Meters GetDistanceTravelled(void) const;
        types::Watts GetCommandedPower(void) const;
        double GetServiceBrake(void) const;
        double GetGrade(void) const;
        bool GetEmergencyBrake(void) const;
        bool GetHeadLights(void) const;
        bool GetInteriorLights(void) const;
        bool GetLeftDoors(void) const;
        bool GetRightDoors(void) const;
        bool GetBrakeFailure(void) const;
        bool GetSignalPickupFailure(void) const;
        bool GetEngineFailure(void) const;
        types::DegreesFahrenheit GetCommandedInternalTemperature(void) const;
        types::DegreesFahrenheit GetActualInternalTemperature(void) const;
        types::Meters GetAuthority(void) const;

        // Implementations for setters
        void SetCommandedSpeed(const types::MetersPerSecond speed);
        void SetDriverSpeed(const types::MilesPerHour speed);
        void SetCurrentSpeed(const types::MetersPerSecond speed);
        void SetServiceBrake(const double percentage);
        void SetEmergencyBrake(const bool state);
        void SetHeadLights(const bool state);
        void SetInteriorLights(const bool state);
        void SetLeftDoors(const bool state);
        void SetRightDoors(const bool state);
        void SetBrakeFailure(const bool state);
        void SetSignalPickupFailure(const bool state);
        void SetEngineFailure(const bool state);
        void SetCommandedInternalTemperature(const types::DegreesFahrenheit temperature);
        void SetActualInternalTemperature(const types::DegreesFahrenheit temperature);
        void SetAuthority(const types::Meters authority);
        void SetArrived(const bool arrived);
        void SetKP(const uint16_t ki);
        void setKI(const uint16_t kp);

        //local functions
        void CalculateCommandedPower(void);
        void UpdateDistanceTravelled(long interval); // NNF-181 TODO: Update the interval application to make use of Tick Source
        void CalculateServiceBrake(types::MetersPerSecond speed_difference);

        /*
         *
         *** RED LINE MAP IMPLEMENTATION ***
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
        //          index 5: underground            (0: no, 1: yes)

        const std::unordered_map<int, std::vector<double>> red_block_data_map_ = {
        {1,  {50, 0.5, 40, 0.25, 0.25, 0}},
        {2,  {50, 1, 40, 0.50, 0.75, 0}},
        {3,  {50, 1.5, 40, 0.75, 1.50, 0}},
        {4,  {50, 2, 40, 1.00, 2.50, 0}},
        {5,  {50, 1.5, 40, 0.75, 3.25, 0}},
        {6,  {50, 1, 40, 0.50, 3.75, 0}},
        {7,  {75, 0.5, 40, 0.38, 4.13, 0}},
        {8,  {75, 0, 40, 0.00, 4.13, 0}},
        {9,  {75, 0, 40, 0.00, 4.13, 0}},
        {10, {75, 0, 40, 0.00, 4.13, 0}},
        {11, {75, -0.5, 40, -0.38, 3.75, 0}},
        {12, {75, -1, 40, -0.75, 3.00, 0}},
        {13, {70, -2, 40, -1.40, 1.60, 0}},
        {14, {60, -1.25, 40, -0.75, 0.85, 0}},
        {15, {60, -1, 40, -0.60, 0.25, 0}},
        {16, {50, -0.5, 40, -0.25, 0.00, 0}},
        {17, {200, -0.5, 55, -1.00, -1.00, 0}},
        {18, {400, -0.06025, 70, -0.24, -1.24, 0}},
        {19, {400, 0, 70, 0.00, -1.24, 0}},
        {20, {200, 0, 70, 0.00, -1.24, 0}},
        {21, {100, 0, 55, 0.00, -1.24, 0}},
        {22, {100, 0, 55, 0.00, -1.24, 0}},
        {23, {100, 0, 55, 0.00, -1.24, 0}},
        {24, {50, 0, 70, 0.00, -1.24, 1}},
        {25, {50, 0, 70, 0.00, -1.24, 1}},
        {26, {50, 0, 70, 0.00, -1.24, 1}},
        {27, {50, 0, 70, 0.00, -1.24, 1}},
        {28, {50, 0, 70, 0.00, -1.24, 1}},
        {29, {60, 0, 70, 0.00, -1.24, 1}},
        {30, {60, 0, 70, 0.00, -1.24, 1}},
        {31, {50, 0, 70, 0.00, -1.24, 1}},
        {32, {50, 0, 70, 0.00, -1.24, 1}},
        {33, {50, 0, 70, 0.00, -1.24, 1}},
        {34, {50, 0, 70, 0.00, -1.24, 1}},
        {35, {50, 0, 70, 0.00, -1.24, 1}},
        {36, {50, 0, 70, 0.00, -1.24, 1}},
        {37, {50, 0, 70, 0.00, -1.24, 1}},
        {38, {50, 0, 70, 0.00, -1.24, 1}},
        {39, {50, 0, 70, 0.00, -1.24, 1}},
        {40, {60, 0, 70, 0.00, -1.24, 1}},
        {41, {60, 0, 70, 0.00, -1.24, 1}},
        {42, {50, 0, 70, 0.00, -1.24, 1}},
        {43, {50, 0, 70, 0.00, -1.24, 1}},
        {44, {50, 0, 70, 0.00, -1.24, 1}},
        {45, {50, 0, 70, 0.00, -1.24, 1}},
        {46, {75, 0, 70, 0.00, -1.24, 1}},
        {47, {75, 0, 70, 0.00, -1.24, 0}},
        {48, {75, 0, 70, 0.00, -1.24, 0}},
        {49, {50, 0, 60, 0.00, -1.24, 0}},
        {50, {50, 0, 60, 0.00, -1.24, 0}},
        {51, {50, 0, 55, 0.00, -1.24, 0}},
        {52, {43.2, 0, 55, 0.00, -1.24, 0}},
        {53, {50, 0, 55, 0.00, -1.24, 0}},
        {54, {50, 0, 55, 0.00, -1.24, 0}},
        {55, {75, 0.5, 55, 0.38, -0.87, 0}},
        {56, {75, 0.5, 55, 0.38, -0.49, 0}},
        {57, {75, 0.5, 55, 0.38, -0.12, 0}},
        {58, {75, 1, 55, 0.75, 0.63, 0}},
        {59, {75, 0.5, 55, 0.38, 1.01, 0}},
        {60, {75, 0, 55, 0.00, 1.01, 0}},
        {61, {75, -0.5, 55, -0.38, 0.63, 0}},
        {62, {75, -1, 55, -0.75, -0.12, 0}},
        {63, {75, -1, 55, -0.75, -0.87, 0}},
        {64, {75, -0.5, 55, -0.38, -1.24, 0}},
        {65, {75, 0, 55, 0.00, -1.24, 0}},
        {66, {75, 0, 55, 0.00, -1.24, 0}},
        {67, {50, 0, 55, 0.00, -1.24, 1}},
        {68, {50, 0, 55, 0.00, -1.24, 1}},
        {69, {50, 0, 55, 0.00, -1.24, 1}},
        {70, {50, 0, 55, 0.00, -1.24, 1}},
        {71, {50, 0, 55, 0.00, -1.24, 1}},
        {72, {50, 0, 55, 0.00, -1.24, 1}},
        {73, {50, 0, 55, 0.00, -1.24, 1}},
        {74, {50, 0, 55, 0.00, -1.24, 1}},
        {75, {50, 0, 55, 0.00, -1.24, 1}},
        {76, {50, 0, 55, 0.00, -1.24, 1}}
        };

        // map for block infrastructure data -->
        // key:
        //          block number
        // values:
        //          index 0: infrastructure string
        //          index 1: station side

        const std::unordered_map<int, std::vector<std::string>> red_infrastructure_data_map_ = {
        {7,  {"STATION: SHADYSIDE", "Left/Right"}},
        {9,  {"SWITCH TO/FROM YARD (75-yard)", ""}},
        {11, {"RAILWAY CROSSING", ""}},
        {15, {"SWITCH (15-16; 1-16)", ""}},
        {16, {"STATION: HERRON AVE", "Left/Right"}},
        {21, {"STATION; SWISSVILLE", "Left/Right"}},
        {25, {"STATION; PENN STATION", "Left/Right"}},
        {27, {"SWITCH (27-28; 27-76)", ""}},
        {32, {"SWITCH (32-33; 33-72)", ""}},
        {35, {"STATION; STEEL PLAZA", "Left/Right"}},
        {38, {"SWITCH (38-39; 38-71)", ""}},
        {43, {"SWITCH (43-44; 44-67)", ""}},
        {45, {"STATION; FIRST AVE", "Left/Right"}},
        {47, {"RAILWAY CROSSING", ""}},
        {48, {"STATION; STATION SQUARE", "Left/Right"}},
        {52, {"SWITCH (52-53; 52-66)", ""}},
        {60, {"STATION; SOUTH HILLS JUNCTION", "Left/Right"}}
        };


        /*
         *
         *** GREEN LINE MAP IMPLEMENTATION ***
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
        //          index 5: underground            (0: no, 1: yes)

        std::unordered_map<int, std::vector<double>> green_block_data_map_ = {
        {1,  {100, 0.5, 45, 0.5, 0.5, 0}},
        {2,  {100, 1, 45, 1, 1.5, 0}},
        {3,  {100, 1.5, 45, 1.5, 3, 0}},
        {4,  {100, 2, 45, 2, 5, 0}},
        {5,  {100, 3, 45, 3, 8, 0}},
        {6,  {100, 4, 45, 4, 12, 0}},
        {7,  {100, 5, 45, 5, 17, 0}},
        {8,  {100, 0, 45, 0, 17, 0}},
        {9,  {100, -5, 45, -5, 12, 0}},
        {10, {100, -4.5, 45, -4.5, 7.5, 0}},
        {11, {100, -4, 45, -4, 3.5, 0}},
        {12, {100, -3, 45, -3, 0.5, 0}},
        {13, {150, 0, 70, 0, 0.5, 0}},
        {14, {150, 0, 70, 0, 0.5, 0}},
        {15, {150, 0, 70, 0, 0.5, 0}},
        {16, {150, 0, 70, 0, 0.5, 0}},
        {17, {150, 0, 60, 0, 0.5, 0}},
        {18, {150, 0, 60, 0, 0.5, 0}},
        {19, {150, 0, 60, 0, 0.5, 0}},
        {20, {150, 0, 60, 0, 0.5, 0}},
        {21, {300, 0, 70, 0, 0.5, 0}},
        {22, {300, 0, 70, 0, 0.5, 0}},
        {23, {300, 0, 70, 0, 0.5, 0}},
        {24, {300, 0, 70, 0, 0.5, 0}},
        {25, {200, 0, 70, 0, 0.5, 0}},
        {26, {100, 0, 70, 0, 0.5, 0}},
        {27, {50, 0, 30, 0, 0.5, 0}},
        {28, {50, 0, 30, 0, 0.5, 0}},
        {29, {50, 0, 30, 0, 0.5, 0}},
        {30, {50, 0, 30, 0, 0.5, 0}},
        {31, {50, 0, 30, 0, 0.5, 0}},
        {32, {50, 0, 30, 0, 0.5, 0}},
        {33, {50, 0, 30, 0, 0.5, 0}},
        {34, {50, 0, 30, 0, 0.5, 0}},
        {35, {50, 0, 30, 0, 0.5, 0}},
        {36, {50, 0, 30, 0, 0.5, 1}},
        {37, {50, 0, 30, 0, 0.5, 1}},
        {38, {50, 0, 30, 0, 0.5, 1}},
        {39, {50, 0, 30, 0, 0.5, 1}},
        {40, {50, 0, 30, 0, 0.5, 1}},
        {41, {50, 0, 30, 0, 0.5, 1}},
        {42, {50, 0, 30, 0, 0.5, 1}},
        {43, {50, 0, 30, 0, 0.5, 1}},
        {44, {50, 0, 30, 0, 0.5, 1}},
        {45, {50, 0, 30, 0, 0.5, 1}},
        {46, {50, 0, 30, 0, 0.5, 1}},
        {47, {50, 0, 30, 0, 0.5, 1}},
        {48, {50, 0, 30, 0, 0.5, 1}},
        {49, {50, 0, 30, 0, 0.5, 1}},
        {50, {50, 0, 30, 0, 0.5, 1}},
        {51, {50, 0, 30, 0, 0.5, 1}},
        {52, {50, 0, 30, 0, 0.5, 1}},
        {53, {50, 0, 30, 0, 0.5, 1}},
        {54, {50, 0, 30, 0, 0.5, 1}},
        {55, {50, 0, 30, 0, 0.5, 1}},
        {56, {50, 0, 30, 0, 0.5, 1}},
        {57, {50, 0, 30, 0, 0.5, 1}},
        {58, {50, 0, 30, 0, 0.5, 0}},
        {59, {50, 0, 30, 0, 0.5, 0}},
        {60, {50, 0, 30, 0, 0.5, 0}},
        {61, {50, 0, 30, 0, 0.5, 0}},
        {62, {50, 0, 30, 0, 0.5, 0}},
        {63, {100, 0, 70, 0, 0.5, 0}},
        {64, {100, 0, 70, 0, 0.5, 0}},
        {65, {200, 0, 70, 0, 0.5, 0}},
        {66, {200, 0, 70, 0, 0.5, 0}},
        {67, {100, 0, 40, 0, 0.5, 0}},
        {68, {100, 0, 40, 0, 0.5, 0}},
        {69, {100, 0, 40, 0, 0.5, 0}},
        {70, {100, 0, 40, 0, 0.5, 0}},
        {71, {100, 0, 40, 0, 0.5, 0}},
        {72, {100, 0, 40, 0, 0.5, 0}},
        {73, {100, 0, 40, 0, 0.5, 0}},
        {74, {100, 0, 40, 0, 0.5, 0}},
        {75, {100, 0, 40, 0, 0.5, 0}},
        {76, {100, 0, 40, 0, 0.5, 0}},
        {77, {300, 0, 70, 0, 0.5, 0}},
        {78, {300, 0, 70, 0, 0.5, 0}},
        {79, {300, 0, 70, 0, 0.5, 0}},
        {80, {300, 0, 70, 0, 0.5, 0}},
        {81, {300, 0, 70, 0, 0.5, 0}},
        {82, {300, 0, 70, 0, 0.5, 0}},
        {83, {300, 0, 70, 0, 0.5, 0}},
        {84, {300, 0, 70, 0, 0.5, 0}},
        {85, {300, 0, 70, 0, 0.5, 0}},
        {86, {100, 0, 25, 0, 0.5, 0}},
        {87, {86.6, 0, 25, 0, 0.5, 0}},
        {88, {100, 0, 25, 0, 0.5, 0}},
        {89, {75, -0.5, 25, -0.375, 0.125, 0}},
        {90, {75, -1, 25, -0.75, -0.625, 0}},
        {91, {75, -2, 25, -1.5, -2.125, 0}},
        {92, {75, 0, 25, 0, -2.125, 0}},
        {93, {75, 2, 25, 1.5, -0.625, 0}},
        {94, {75, 1, 25, 0.75, 0.125, 0}},
        {95, {75, 0.5, 25, 0.375, 0.5, 0}},
        {96, {75, 0, 25, 0, 0.5, 0}},
        {97, {75, 0, 25, 0, 0.5, 0}},
        {98, {75, 0, 25, 0, 0.5, 0}},
        {99, {75, 0, 25, 0, 0.5, 0}},
        {100, {75, 0, 25, 0, 0.5, 0}},
        {101, {35, 0, 26, 0, 0.5, 0}},
        {102, {100, 0, 28, 0, 0.5, 0}},
        {103, {100, 0, 28, 0, 0.5, 0}},
        {104, {80, 0, 28, 0, 0.5, 0}},
        {105, {100, 0, 28, 0, 0.5, 0}},
        {106, {100, 0, 28, 0, 0.5, 0}},
        {107, {90, 0, 28, 0, 0.5, 0}},
        {108, {100, 0, 28, 0, 0.5, 0}},
        {109, {100, 0, 28, 0, 0.5, 0}},
        {110, {100, 0, 30, 0, 0.5, 0}},
        {111, {100, 0, 30, 0, 0.5, 0}},
        {112, {100, 0, 30, 0, 0.5, 0}},
        {113, {100, 0, 30, 0, 0.5, 0}},
        {114, {162, 0, 30, 0, 0.5, 0}},
        {115, {100, 0, 30, 0, 0.5, 0}},
        {116, {100, 0, 30, 0, 0.5, 0}},
        {117, {50, 0, 15, 0, 0.5, 0}},
        {118, {50, 0, 15, 0, 0.5, 0}},
        {119, {40, 0, 15, 0, 0.5, 0}},
        {120, {50, 0, 15, 0, 0.5, 0}},
        {121, {50, 0, 15, 0, 0.5, 0}},
        {122, {50, 0, 20, 0, 0.5, 1}},
        {123, {50, 0, 20, 0, 0.5, 1}},
        {124, {50, 0, 20, 0, 0.5, 1}},
        {125, {50, 0, 20, 0, 0.5, 1}},
        {126, {50, 0, 20, 0, 0.5, 1}},
        {127, {50, 0, 20, 0, 0.5, 1}},
        {128, {50, 0, 20, 0, 0.5, 1}},
        {129, {50, 0, 20, 0, 0.5, 1}},
        {130, {50, 0, 20, 0, 0.5, 1}},
        {131, {50, 0, 20, 0, 0.5, 1}},
        {132, {50, 0, 20, 0, 0.5, 1}},
        {133, {50, 0, 20, 0, 0.5, 1}},
        {134, {50, 0, 20, 0, 0.5, 1}},
        {135, {50, 0, 20, 0, 0.5, 1}},
        {136, {50, 0, 20, 0, 0.5, 1}},
        {137, {50, 0, 20, 0, 0.5, 1}},
        {138, {50, 0, 20, 0, 0.5, 1}},
        {139, {50, 0, 20, 0, 0.5, 1}},
        {140, {50, 0, 20, 0, 0.5, 1}},
        {141, {50, 0, 20, 0, 0.5, 1}},
        {142, {50, 0, 20, 0, 0.5, 1}},
        {143, {50, 0, 20, 0, 0.5, 1}},
        {144, {50, 0, 20, 0, 0.5, 0}},
        {145, {50, 0, 20, 0, 0.5, 0}},
        {146, {50, 0, 20, 0, 0.5, 0}},
        {147, {50, 0, 20, 0, 0.5, 0}},
        {148, {184, 0, 20, 0, 0.5, 0}},
        {149, {40, 0, 20, 0, 0.5, 0}},
        {150, {35, 0, 20, 0, 0.5, 0}}
        };

        // map for block infrastructure data -->
        // key:
        //          block number
        // values:
        //          index 0: infrastructure string
        //          index 1: station side

        std::unordered_map<int, std::vector<std::string>> green_infrastructure_data_map_ = {
        {2,  {"STATION; PIONEER", "Left"}},
        {9,  {"STATION; EDGEBROOK", "Left"}},
        {12, {"SWITCH (12-13; 1-13)", ""}},
        {16, {"STATION", "Left/Right"}},
        {19, {"RAILWAY CROSSING", ""}},
        {22, {"STATION; WHITED", "Left/Right"}},
        {28, {"SWITCH (28-29; 150-28)", ""}},
        {31, {"STATION; SOUTH BANK", "Left"}},
        {39, {"STATION; CENTRAL", "Right"}},
        {48, {"STATION; INGLEWOOD", "Right"}},
        {57, {"STATION; OVERBROOK", "Right"}},
        {58, {"SWITCH TO YARD (57-yard)", ""}},
        {62, {"SWITCH FROM YARD (Yard-63)", ""}},
        {65, {"STATION; GLENBURY", "Right"}},
        {73, {"STATION; DORMONT", "Right"}},
        {76, {"SWITCH (76-77;77-101)", ""}},
        {77, {"STATION; MT LEBANON", "Left/Right"}},
        {85, {"SWITCH (85-86; 100-85)", ""}},
        {88, {"STATION; POPLAR", "Left"}},
        {96, {"STATION; CASTLE SHANNON", "Left"}},
        {105, {"STATION; DORMONT", "Right"}},
        {108, {"RAILWAY CROSSING", ""}},
        {114, {"STATION; GLENBURY", "Right"}},
        {123, {"STATION; OVERBROOK", "Right"}},
        {132, {"STATION; INGLEWOOD", "Left"}},
        {141, {"STATION; CENTRAL", "Right"}}
        };

    private:

        float integral_sum_;
        uint16_t kp_;
        uint16_t ki_;

        types::KilometersPerHour train_max_speed_;
        types::Watts max_power_;
        types::MetersPerSecond commanded_speed_;
        types::MetersPerSecond driver_speed_;
        types::MetersPerSecond current_speed_;
        double service_brake_percentage_;
        types::Watts commanded_power_;
        types::Meters authority_;
        bool emergency_brake_;
        bool headlights_;
        bool interior_lights_;
        bool left_door_;
        bool right_door_;
        bool brake_failure_;
        bool signal_pickup_failure_;
        bool engine_failure_;
        types::DegreesFahrenheit commanded_internal_temperature_;
        types::DegreesFahrenheit actual_internal_temperature_;
        types::Meters distance_travelled_;
        bool arrived_;
};

} // namespace train_controller

#endif // TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_H