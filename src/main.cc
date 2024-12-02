#include <thread>

#include <slint.h>
#include <unistd.h>

#include "launcher.h"
#include "simulator.h"
#include "ctc.h"
#include "types.h"
#include "ctc_callback_handler.h"
#include "channel.h"

#ifdef _WIN32
#include <windows.h>
#include <iostream>

void AttachConsoleToApp() {
    AllocConsole(); // Allocates a new console
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout); // Redirects stdout to the console
    freopen_s(&fp, "CONOUT$", "w", stderr); // Redirects stderr to the console
    freopen_s(&fp, "CONIN$", "r", stdin);  // Redirects stdin to the console
}
#endif


int main(void)
{
    #ifdef _WIN32
    AttachConsoleToApp(); 
    #endif
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
    ctc::setup_ui(ctc_ui); 

    std::thread worker_thread([&]
        {   
            TickSource tick_source("08:00:00", std::chrono::milliseconds(1));
            tick_source.Start();
            usleep(5e5);
            std::shared_ptr<TickSource> clock = std::make_shared<TickSource>(tick_source);
            ctc::Ctc ctc_office(clock);
            
            
            while (keep_running.load())
            {
                ctc::backend_handler(ctc_office, ctc_ui);
            }
        });

    // Stop work_thread when launcher is closed
    launcher_ui->window().on_close_requested([&] {
        keep_running.store(false);
        return slint::CloseRequestResponse::HideWindow;
    });

    launcher_ui->run();
    worker_thread.join();

    return 0;
}