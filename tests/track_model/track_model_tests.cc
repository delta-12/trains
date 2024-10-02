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

int main(void)
{
    //initial testing
    track_model::SoftwareTrackModel trackModel;

    //track circuit fail
    types::BlockId blockID = 1;
    bool occu;

    trackModel.SetTrackCircuitFailure(blockID, 1);

    trackModel.GetBlockOccupancy(blockID, occu);

    if (occu==1)
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