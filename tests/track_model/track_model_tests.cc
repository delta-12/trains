/*****************************************************************************
 * @file track_model_tests.cc
 *
 * @brief Short-term testing for the Track Model backend.
 *****************************************************************************/

//#include "track_model_ui.h"
//#include "train_model.h"
#include "track_model.h"
//#include <cstdint>
#include <iostream>
#include "types.h"
#include <memory>
//#include "track_model.cc"

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