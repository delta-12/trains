#include <chrono>
#include <iostream>
#include <thread>

#include <slint.h>

#include "launcher.h"

int main(int argc, char **argv)
{
    auto launcher_ui = Launcher::create();
    auto ctc_ui = CtcWindow::create();
    auto wayside_controller_ui = WaysideControllerWindow::create();
    auto track_model_ui = TrackModelWindow::create();
    auto train_model_ui = TrainModelWindow::create();
    auto train_controller_ui = TrainControllerWindow::create();

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