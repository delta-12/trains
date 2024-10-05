/*****************************************************************************
 * @file train_controller_tests.cc
 *
 * @brief Short-term testing for the Train Controller backend.
 *****************************************************************************/

#include "train_controller.h"
#include "train_controller_ui.h"
#include <chrono>
#include <memory>
#include <slint_timer.h>
#include <thread>

int main(void)
{
    auto train_controller_ui = ui::TrainControllerUi::create();

    // operation mode

    train_controller_ui->on_request_automatic_mode([&]{
        if (train_controller_ui->get_operation_mode_status() == "Manual")
        {
            train_controller_ui->set_operation_status_message("In Manual, cannot switch to Automatic");

            // timer to clear the message after 3 seconds
            auto timer = std::make_shared<slint::Timer>();
            timer->single_shot(std::chrono::seconds(3), [train_controller_ui, timer]() {
                train_controller_ui->set_operation_status_message("");
            });
        }
    });
    train_controller_ui->on_request_manual_mode([&]{
        train_controller_ui->set_operation_mode_status("Manual");
        });

    train_controller_ui->run();

    // failures

    //train_controller_ui->on

    return 0;
}