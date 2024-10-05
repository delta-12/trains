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

    // failures

    train_controller_ui->on_request_engine_fail([&] {
        if (train_controller_ui->get_engine_status() == 0)
            train_controller_ui->set_engine_status(1);
        else
            train_controller_ui->set_engine_status(0);
    });
    train_controller_ui->on_request_brake_fail([&] {
        if (train_controller_ui->get_brake_status() == 0)
            train_controller_ui->set_brake_status(1);
        else
            train_controller_ui->set_brake_status(0);
    });
    train_controller_ui->on_request_signal_fail([&] {
        if (train_controller_ui->get_signal_status() == 0)
            train_controller_ui->set_signal_status(1);
        else
            train_controller_ui->set_signal_status(0);
    });

    train_controller_ui->run();

    return 0;
}