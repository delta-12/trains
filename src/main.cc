#include <atomic>
#include <cstring>
#include <thread>

#include <slint.h>

#include "launcher.h"
#include "simulator.h"
#include "wayside_controller_port.h"
#include "random_number_generator.h"

int main(void)
{
    std::atomic_bool running(true);
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

    std::thread worker_thread([&]
    {
        RandomNumberGenerator random_number_generator;

        wayside_controller::WaysideControllerPort wayside_controller_port("/dev/ttyACM0");
        // const char * test_data = "5,10\n";
        // wayside_controller_port.Send((uint8_t*)test_data, strlen(test_data));
        char buf[6];        

        // Main backend loop here
        while (running.load())
        {

            int i = random_number_generator.generate(10);
            int j = random_number_generator.generate(10);

            snprintf(buf, 6, "%d,%d\n", i, j);
            wayside_controller_port.Send((uint8_t*)buf, strlen(buf));

            std::this_thread::sleep_for (std::chrono::seconds(3));
        }
    });

    launcher_ui->run();
    running.store(false);
    worker_thread.join();

    return 0;
}