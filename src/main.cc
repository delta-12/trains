#include <memory>
#include <thread>

#include <slint.h>

#include "launcher.h"

#include "block_builder.h"
#include "csv_parser.h"
#include "controller_handler.h"
#include "controller_port.h"
#include "simulator.h"
#include "tcp_server.h"
#include "tick_source.h"
#include "track_model.h"
#include "train_model.h"

int main(void)
{
    std::atomic_bool running(true);
    simulator::Simulator world;
    auto launcher_ui = ui::Launcher::create();
    auto ctc_ui = ui::CtcUi::create();
    auto wayside_controller_ui = ui::WaysideControllerUi::create();
    auto track_model_ui = ui::TrackModelUi::create();
    auto train_model_ui = ui::TrainModelUi::create();
    auto train_controller_ui = ui::TrainControllerUi::create();

    launcher_ui->on_launch_ctc_window([&]
                                      { ctc_ui->show(); });
    launcher_ui->on_launch_wayside_controller_window([&]
                                                     { wayside_controller_ui->show(); });
    launcher_ui->on_launch_track_model_window([&]
                                              { track_model_ui->show(); });
    launcher_ui->on_launch_train_model_window([&]
                                              { train_model_ui->show(); });
    launcher_ui->on_launch_train_controller_window([&]
                                                   { train_controller_ui->show(); });

    std::thread worker_thread([&]
                              {
        std::filesystem::path           base_path = std::filesystem::current_path();
        std::filesystem::path           path      = base_path / "tests" / "common" / "test_csv" / "green_line_path.csv";
        std::filesystem::path           path2     = base_path / "tests" / "common" / "test_csv" / "green_line_track_layout.csv";
        CsvParser                       parser(path);
        CsvParser                       parser2(path2);
        BlockBuilder                    bb(parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
        BlockBuilder                    bb2(parser2.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
        
        // Create clock
        std::shared_ptr<TickSource> tick_source = std::make_shared<TickSource>();

        // Create CTC
        ctc::Ctc              ctc_office;
        std::filesystem::path schedule_path      = base_path / "tests" / "common" / "test_csv" / "green_line_schedule.csv";
        ctc_office.SetScheduleFilePath(schedule_path);
        ctc_office.SetTrackLayout();

        // Create train
        std::shared_ptr<train_model::TrainModel> train = std::make_shared<train_model::SoftwareTrainModel>(tick_source);

        // Create track
        std::shared_ptr<track_model::SoftwareTrackModel> track = std::make_shared<track_model::SoftwareTrackModel>();
        track->SetTrackLayout(types::TrackId::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());

        // Add track and train to simulator
        world.AddTrackModel(track);
        world.AddTrainModel(track->GetTrackId(), train);

        // Create controller handler and start TCP server
        controller_network::ControllerHandler<1024> controller_handler;
        controller_network::TcpServer tcp_server(8080, [&](std::shared_ptr<types::Port> port){
            controller_handler.AddPort(controller_network::BuildBasicControllerPort<1024>(port));
        });

        // Main loop
        while (running.load())
        {
            // TODO run simulator update, controller handler update, and wayside update

            tcp_server.RunFor(std::chrono::milliseconds(10));
            controller_handler.Update(ctc_office, world);
            world.Update();
        } });

    launcher_ui->run();
    running.store(false);
    worker_thread.join();

    return 0;
}