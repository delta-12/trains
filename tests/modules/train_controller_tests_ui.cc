/*****************************************************************************
 * @file train_controller_tests.cc
 *
 * @brief Short-term testing for the Train Controller backend.
 *****************************************************************************/

#include "train_controller.h"
#include "train_controller_ui.h"

int main(void)
{
    auto train_controller_ui = ui::TrainControllerUi::create();

    train_controller_ui->run();

    return 0;
}