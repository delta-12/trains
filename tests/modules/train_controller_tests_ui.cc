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

    train_controller_ui->on_request_automatic_mode([&]{
        if (train_controller_ui->get_operation_mode_status() == "Manual")
            train_controller_ui->set_operation_mode_status("In Manual, cannot switch to Automatic");
    });
    train_controller_ui->on_request_manual_mode([&]{
        train_controller_ui->set_operation_mode_status("Manual");
        });

    train_controller_ui->run();

    return 0;
}