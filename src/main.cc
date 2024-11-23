#include <thread>

#include <slint.h>

#include "launcher.h"
#include "simulator.h"
#include "ctc.h"
#include "types.h"
#include "ctc_callback_handler.h"
#include "channel.h"


int main(void)
{
    // Prevent worker_thread keep running after application window is closed
    std::atomic<bool> keep_running(true);

    simulator::Simulator world;
    auto                 launcher_ui           = ui::Launcher::create();
    auto                 ctc_ui                = ui::CtcUi::create();
    auto                 wayside_controller_ui = ui::WaysideControllerUi::create();
    auto                 track_model_ui        = ui::TrackModelUi::create();
    auto                 train_model_ui        = ui::TrainModelUi::create();
    auto                 train_controller_ui   = ui::TrainControllerUi::create();

    launcher_ui->on_launch_ctc_window([&]
    {
        ctc_ui->show();
    });
    launcher_ui->on_launch_wayside_controller_window([&]
    {
        wayside_controller_ui->show();
    });
    launcher_ui->on_launch_track_model_window([&]
    {
        track_model_ui->show();
    });
    launcher_ui->on_launch_train_model_window([&]
    {
        train_model_ui->show();
    });
    launcher_ui->on_launch_train_controller_window([&]
    {
        train_controller_ui->show();
    });

    // Setting Up CTC
    ctc::Ctc ctc_office(types::TrackId::TRACKID_GREEN);
    ctc::setup_ui(ctc_ui, ctc_office);

    std::thread worker_thread([&]
        {
                              while (keep_running.load())
                              {
                                  ctc::backend_handler(ctc_office, ctc_ui);
                              }
        });

    // Stop work_thread when launcher is closed
    launcher_ui->window().on_close_requested([&] {
        keep_running = false;
        return slint::CloseRequestResponse::HideWindow;
    });

    launcher_ui->run();
    worker_thread.join();

    return 0;
}
