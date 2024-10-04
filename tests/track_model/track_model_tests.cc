/*****************************************************************************
 * @file track_model_tests.cc
 *
 * @brief Short-term testing for the Track Model backend.
 *****************************************************************************/

//#include "track_model_ui.h"
#include "train_model.h"
#include "track_model.h"
//#include <cstdint>
#include <iostream>
#include "types.h"
#include <memory>
//#include "track_model.cc"


//MOCK TRAIN MODEL
class MockTrainModel : public train_model::TrainModel {
    public: 
        void SetTrainId(const types::TrainId train);
        types::TrainId GetTrainId(void) const;
        void SetEmergencyBrake(const bool emergency_brake);
        void SetEngineFailure(const bool engine_failure);
        void SetBrakeFailure(const bool brake_failure);
        void SetSignalPickupFailure(const bool signal_pickup_failure);
        bool GetBrakeFailure(void) const;
        std::size_t GetBeaconData(types::BeaconData &data) const;
        types::Meters GetAuthority(void) const;
        types::MetersPerSecond GetCommandedSpeed(void) const;
        types::MetersPerSecond GetActualSpeed(void) const;
        bool GetEngineFailure(void) const;
        bool GetSignalPickupFailure(void) const;
        types::Watts GetActualPower(void) const;
        types::DegreesFahrenheit GetActualInternalTemperature(void) const;
        types::Polarity GetTrackPolarity(void) const;
        void SetStationAnnouncement(const std::string &announcement);
        void SetGrade(const float grade);
        void SetBrake(const float brake);
        void SetHeadlights(const bool on);
        void SetInternalLights(const bool on);
        void SetLeftDoorsState(const bool open);
        void SetRightDoorsState(const bool open);
        void SetCommandedPower(const types::Watts power);
        void SetCommandedInternalTemperature(const types::DegreesFahrenheit degrees);
        void SetDistanceTraveled(const types::Meters distance);
        uint16_t GetPassengersDeboarding(void);
        types::Meters GetDistanceTraveled(void);
        void SetCommandedSpeed(const types::MetersPerSecond speed);
        void SetAuthority(const types::Meters meters);
        void SetPassengersBoarding(const uint16_t passegners);
        void SetTrackPolarity(const types::Polarity polarity);
        void SetBeaconData(const types::BeaconData &data, std::size_t &size);
};



int main(void)
{
    //initial testing
    track_model::SoftwareTrackModel trackModel;

    //track circuit fail
    types::BlockId blockID = 1;
    bool occu;
    bool occu2;
    bool occu3;

    if (trackModel.SetPowerFailure(blockID, 1) == types::ERROR_INVALID_BLOCK)
    {
        std::cout << "TC ERROR" << std::endl;
    }

    if (trackModel.GetBlockOccupancy(blockID, occu) == types::ERROR_INVALID_BLOCK)
    {
        std::cout << "BO ERROR" << std::endl;
    }

    blockID=2;

    if (trackModel.SetBrokenRail(blockID, 1) == types::ERROR_INVALID_BLOCK)
    {
        std::cout << "BR ERROR" << std::endl;
    }

    if (trackModel.GetBlockOccupancy(blockID, occu2) == types::ERROR_INVALID_BLOCK)
    {
        std::cout << "BO2 ERROR" << std::endl;
    }

    blockID=3;

    if (trackModel.GetBlockOccupancy(blockID, occu3) == types::ERROR_INVALID_BLOCK)
    {
        std::cout << "BO2 ERROR" << std::endl;
    }

    if (occu==1 && occu2==1 && occu3==0)
    {
        std::cout << "Occupancy success" << std::endl;
    }
    else
    {
        std::cout << "Occupancy FAIL!!!" << std::endl;
    }

    //auto track_model_ui = ui::TrackModelUi::create();

    //track_model_ui->run();

    return 0;
}