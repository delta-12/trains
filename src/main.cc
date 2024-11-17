#include <memory>
#include <thread>

#include <slint.h>

#include "launcher.h"

#include "block_builder.h"
#include "csv_parser.h"
#include "simulator.h"
#include "tick_source.h"
#include "track_model.h"
#include "train_model.h"
#include "wayside_controller_port.h"

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
        std::filesystem::path           base_path = std::filesystem::current_path();
        std::filesystem::path           path      = base_path / "tests" / "common" / "test_csv" / "green_line_path.csv";
        std::filesystem::path           path2     = base_path / "tests" / "common" / "test_csv" / "green_line_track_layout.csv";
        CsvParser                       parser(path);
        CsvParser                       parser2(path2);
        BlockBuilder                    bb(parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
        BlockBuilder                    bb2(parser2.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
        
        std::shared_ptr<TickSource> tick_source = std::make_shared<TickSource>();
        std::shared_ptr<train_model::TrainModel> train = std::make_shared<train_model::SoftwareTrainModel>(tick_source);
        std::shared_ptr<track_model::SoftwareTrackModel> track = std::make_shared<track_model::SoftwareTrackModel>();

        track->SetTrackLayout(types::TrackId::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());

        world.AddTrackModel(track);
        world.AddTrainModel(track->GetTrackId(), train);
        world.Update();
    });

    launcher_ui->run();
    running.store(false);
    worker_thread.join();

    return 0;
}