#include <memory>
#include <thread>

#include <slint.h>
#include <unistd.h>

#include "launcher.h"

#include "types.h"

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif

#include "block_builder.h"
#include "controller_handler.h"
#include "controller_port.h"
#include "csv_parser.h"
#include "ctc.h"
#include "ctc_callback_handler.h"
#include "green_line_blocks.h"
#include "hardware_wayside_controller_testbench.h"
#include "logger.h"
#include "ring_buffer.h"
#include "simulator.h"
#include "tcp_server.h"
#include "tick_source.h"
#include "track_model.h"
#include "train_model.h"
#include "wayside_controller_handler.h"

#include "asio.hpp"

int main(void)
{
    std::atomic_bool     running(true);
    simulator::Simulator world;
    auto                 launcher_ui                              = ui::Launcher::create();
    auto                 ctc_ui                                   = ui::CtcUi::create();
    auto                 wayside_controller_ui                    = ui::WaysideControllerUi::create();
    auto                 track_model_ui                           = ui::TrackModelUi::create();
    auto                 train_model_ui                           = ui::TrainModelUi::create();
    auto                 train_controller_ui                      = ui::TrainControllerUi::create();
    auto                 hardware_wayside_controller_testbench_ui = ui::HardwareWaysideControllerTestbenchUi::create();

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
    launcher_ui->on_launch_hardware_wayside_controller_testbench_window([&]
    {
        hardware_wayside_controller_testbench_ui->show();
    });

    std::shared_ptr<wayside_controller::HardwareWaysideControllerTestbench> testbench = std::make_shared<wayside_controller::HardwareWaysideControllerTestbench>(hardware_wayside_controller_testbench_ui);

    ctc::setup_ui(ctc_ui);

    std::thread worker_thread([&]
        {

                              std::shared_ptr<TickSource> clock = std::make_shared<TickSource>("08:00:00", std::chrono::milliseconds(1));
                              clock->Start();

                              simulator::Simulator world;
                              ctc::Ctc ctc_office(types::TrackId::TRACKID_GREEN, clock);

                              world.AddTrackModel(testbench);

                              bool connected = false;
                              controller_network::ControllerHandler<1024> controller_handler;
                                controller_network::TcpServer tcp_server(8080, [&](std::shared_ptr<types::Port> port){
                                 controller_handler.AddPort(controller_network::BuildBasicControllerPort<1024>(port));
                                 connected = true;
                              });

                              CsvParser csv_parser(std::filesystem::current_path() / "green_line_schedule.csv");
                              BlockBuilder block_builder(csv_parser.GetRecords(), RecordType::RECORDTYPE_SCHEDULE);
                              controller_handler.SetWaysideLayout(block_builder.GetBlocks());
                              controller_handler.Update(ctc_office, world);

                              // Main loop
                              while (running.load())
                              {

                                  tcp_server.RunFor(std::chrono::milliseconds(10));
                                  world.Update();
                                  testbench->Update(controller_handler, connected);
                                  controller_handler.Update(ctc_office, world);
                                  ctc::backend_handler(ctc_office, ctc_ui);
                              }
        });

    launcher_ui->run();
    running.store(false);
    worker_thread.join();

    return 0;
}
