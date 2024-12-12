#include <memory>
#include <thread>

#include <slint.h>
#include <unistd.h>

#include "launcher.h"

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
#include "types.h"
#include "wayside_controller_handler.h"

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

                              controller_network::ControllerHandler<1024> controller_handler;
                              //   controller_network::TcpServer tcp_server(8080, [&](std::shared_ptr<types::Port> port){
                              //    controller_handler.AddPort(controller_network::BuildBasicControllerPort<1024>(port));
                              // });
                              bool connected = false;
                              controller_network::TcpServer tcp_server(8080, [&connected](std::shared_ptr<types::Port> port) {
                                                                       (void)port;
                                                                       connected = true;
            });

                              CsvParser csv_parser(std::filesystem::current_path() / "green_line_schedule.csv");
                              BlockBuilder block_builder(csv_parser.GetRecords(), RecordType::RECORDTYPE_SCHEDULE);
                              controller_handler.SetWaysideLayout(block_builder.GetBlocks());
                              controller_handler.Update(ctc_office, world);

                              // Software wayside
                              RingBuffer<uint8_t, 1024>                                  buffer_0, buffer_1;
                              wayside_controller::SoftwareWaysideControllerHandler<1024> wayside_controller_handler(1,
                                                                                                                    types::TrackId::TRACKID_GREEN,
                                                                                                                    wayside_controller::kGreenLineBlocksWayside1,
                                                                                                                    wayside_controller::kGreenLineBlockOutputsWayside1,
                                                                                                                    controller_network::BuildSoftwareBasicControllerPort<1024>(buffer_0, buffer_1));
                              controller_handler.AddPort(controller_network::BuildSoftwareBasicControllerPort<1024>(buffer_1, buffer_0));
                              wayside_controller_handler.Connect();

                              //   std::chrono::steady_clock::time_point last_update = std::chrono::steady_clock::now();

                              // Main loop
                              while (running.load())
                              {

                                  tcp_server.RunFor(std::chrono::milliseconds(10));
                                  world.Update();
                                  testbench->Update(controller_handler, connected);
                                  controller_handler.Update(ctc_office, world);

                                  // TODO if connected, update wc
                                  wayside_controller_handler.Update();

                                  ctc::backend_handler(ctc_office, ctc_ui);

                                  //   if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_update).count() > 500)
                                  //   {
                                  //       controller_handler.Update(*testbench, world);

                                  //       last_update = std::chrono::steady_clock::now();
                                  //   }


                              }
        });

    launcher_ui->run();
    running.store(false);
    worker_thread.join();

    return 0;
}
