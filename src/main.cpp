#include <chrono>
#include <iostream>
#include <thread>

#include <slint.h>

#include "launcher.h"
#include "simulator.h"

int main(void)
{
    simulator::Simulator world;
    auto launcher_ui = ui::Launcher::create();
    auto ctc_ui = ui::CtcWindow::create();
    auto wayside_controller_ui = ui::WaysideControllerWindow::create();
    auto track_model_ui = ui::TrackModelWindow::create();
    auto train_model_ui = ui::TrainModelWindow::create();
    auto train_controller_ui = ui::TrainControllerWindow::create();

    launcher_ui->on_launch_ctc_window([&]{
        ctc_ui->show();
    });
    launcher_ui->on_launch_wayside_controller_window([&]{
        wayside_controller_ui->show();
    });
    launcher_ui->on_launch_track_model_window([&]{
        track_model_ui->show();
    });
    launcher_ui->on_launch_train_model_window([&]{
        train_model_ui->show();
    });
    launcher_ui->on_launch_train_controller_window([&]{
        train_controller_ui->show();
    });

    launcher_ui->run();

    return 0;
}