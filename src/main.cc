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
#include "logger.h"
#include "ring_buffer.h"
#include "simulator.h"
#include "tcp_server.h"
#include "tick_source.h"
#include "track_model.h"
#include "train_model.h"
#include "types.h"
#include "wayside_controller_handler.h"

#ifdef _WIN32
#include <windows.h>
#include <iostream>

void AttachConsoleToApp()
{
    AllocConsole(); // Allocates a new console
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout); // Redirects stdout to the console
    freopen_s(&fp, "CONOUT$", "w", stderr); // Redirects stderr to the console
    freopen_s(&fp, "CONIN$", "r", stdin);   // Redirects stdin to the console
}
#endif

int main(void)
{
    #ifdef _WIN32
    AttachConsoleToApp();
    #endif

    std::atomic_bool     running(true);
    simulator::Simulator world;
    auto                 launcher_ui           = ui::Launcher::create();
    auto                 ctc_ui                = ui::CtcUi::create();
    auto                 wayside_controller_ui = ui::WaysideControllerUi::create();
    auto                 track_model_ui        = ui::TrackModelUi::create();
    auto                 train_model_ui        = ui::TrainModelUi::create();
    auto                 train_controller_ui   = ui::TrainControllerUi::create();

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

    // Setting Up CTC
    ctc::Ctc ctc_office(types::TrackId::TRACKID_GREEN);
    ctc::setup_ui(ctc_ui, ctc_office);

    std::thread worker_thread([&]
        {
                              std::filesystem::path base_path = std::filesystem::current_path();
                              std::filesystem::path path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_path.csv";
                              std::filesystem::path path2     = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_track_layout.csv";
                              CsvParser parser(path);
                              CsvParser parser2(path2);
                              BlockBuilder bb(parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
                              BlockBuilder bb2(parser2.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);

                              // Create clock
                              std::shared_ptr<TickSource> tick_source = std::make_shared<TickSource>();

                              // Create train
                              std::shared_ptr<train_model::TrainModel> train = std::make_shared<train_model::SoftwareTrainModel>(tick_source);

                              // Create track
                              std::shared_ptr<track_model::SoftwareTrackModel> track = std::make_shared<track_model::SoftwareTrackModel>();
                              track->SetTrackLayout(types::TrackId::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());

                              // Add track and train to simulator
                              world.AddTrackModel(track);
                              world.AddTrainModel(track->GetTrackId(), train);

                              // Hardware wayside
                              // Create controller handler and start TCP server
                              // controller_network::ControllerHandler<1024> controller_handler;
                              // controller_network::TcpServer tcp_server(8080, [&](std::shared_ptr<types::Port> port){
                              //     controller_handler.AddPort(controller_network::BuildBasicControllerPort<1024>(port));
                              // });

                              // Software wayside
                              std::filesystem::path schedule_path = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_schedule.csv";
                              RingBuffer<uint8_t, 1024>                                  buffer_0, buffer_1;
                              wayside_controller::SoftwareWaysideControllerHandler<1024> wayside_controller_handler(1,
                                                                                                                    types::TrackId::TRACKID_GREEN,
                                                                                                                    wayside_controller::kGreenLineBlocksWayside0,
                                                                                                                    controller_network::BuildSoftwareBasicControllerPort<1024>(buffer_0, buffer_1));
                              CsvParser csv_parser(schedule_path);
                              BlockBuilder block_builder(csv_parser.GetRecords(), RecordType::RECORDTYPE_SCHEDULE);
                              std::vector<types::Block> blocks = bb.GetBlocks();
                              controller_network::ControllerHandler<1024> controller_handler;
                              controller_handler.AddPort(controller_network::BuildSoftwareBasicControllerPort<1024>(buffer_1, buffer_0));
                              controller_handler.SetWaysideLayout(block_builder.GetBlocks());
                              wayside_controller_handler.Connect();
                              controller_handler.Update(ctc_office, world);

                              // Manually dispatch train
                              ctc_office.ManualDispatch(1, 80);

                              train->SetDistanceTraveled(1);


                              std::chrono::steady_clock::time_point last_ui_update = std::chrono::steady_clock::now();

                              // Main loop
                              while (running.load())
                              {
                                  // TODO run simulator update, controller handler update, and wayside update

                                  // tcp_server.RunFor(std::chrono::milliseconds(10));
                                  controller_handler.Update(ctc_office, world);
                                  wayside_controller_handler.Update();
                                  world.Update();

                                  types::MetersPerSecond speed = train->GetCommandedSpeed();
                                  types::Blocks authority      = train->GetAuthority();

                                  if ((speed != 0) || (authority != 0))
                                  {
                                      LOGGER_LOG_DEBUG(std::cout, "MAIN", "Speed: {}, Authority: {}", speed, authority);

                                      train->SetDistanceTraveled(100);
                                  }

                                  // Update UIs
                                  if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_ui_update).count() > 10)
                                  {
                                      ctc::backend_handler(ctc_office, ctc_ui);
                                      last_ui_update = std::chrono::steady_clock::now();

                                      LOGGER_LOG_DEBUG(std::cout, "MAIN", "Updated UI");
                                  }
                              }
        });

    launcher_ui->run();
    running.store(false);
    worker_thread.join();

    return 0;
}
