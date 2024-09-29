/*****************************************************************************
 * @file track_model_tests.cc
 *
 * @brief Short-term testing for the Track Model backend.
 *****************************************************************************/

#include "track_model.h"
#include "track_model_ui.h"

int main(void)
{
    auto track_model_ui = ui::TrackModelUi::create();

    track_model_ui->run();

    return 0;
}