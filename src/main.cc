#include <thread>

#include <slint.h>
#include "channel.h"
#include "wayside_controller.h"
#include "launcher.h"
#include "simulator.h"
#include "wayside_controller_callback_handler.h"

// Prevent worker_thread keep running after application window is closed
std::atomic<bool> keep_running(true);


int main(void)
{
    simulator::Simulator world;
    auto                 launcher_ui           = ui::Launcher::create();
    auto                 ctc_ui                = ui::CtcUi::create();
    auto                 wayside_controller_ui = ui::::create();
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




    std::thread worker_thread([&]
    {
        // Main backend loop here
        {   
            while (keep_running.load())
            {
                ctc::backend_handler(ctc_office, ctc_ui);
            }
        }

    });

    launcher_ui->run();
    //tick_thread.join();
    worker_thread.join();

    return 0;
}

//functionality must occur on the worker thread