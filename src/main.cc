#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include <slint.h>

#include "launcher.h"
#include "simulator.h"
#include "tick_source.h"
#include "track_model.h"
#include "train_model.h"
#include "block_builder.h"
#include "csv_parser.h"
#include "types.h"

int main(void)
{
    std::filesystem::path           base_path = std::filesystem::current_path();
    std::filesystem::path           path      = base_path / "tests" / "common" / "test_csv" / "green_line_path.csv";
    std::filesystem::path           path2     = base_path / "tests" / "common" / "test_csv" / "green_line_track_layout.csv";
    CsvParser                       parser(path);
    CsvParser                       parser2(path2);
    BlockBuilder                    bb(parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    BlockBuilder                    bb2(parser2.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
    types::Block                    block;

    track_model::SoftwareTrackModel track;
    track.SetTrackLayout(types::TrackId::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());

    TickSource clock;
    std::shared_ptr<TickSource> clk = std::make_shared<TickSource>(clock);
    train_model::SoftwareTrainModel train(clk);
    std::shared_ptr<train_model::SoftwareTrainModel> trainptr = std::make_shared<train_model::SoftwareTrainModel>(train);

    track.AddTrainModel(trainptr);
    std::vector<std::shared_ptr<train_model::TrainModel>> trains_vector;
    track.GetTrainModels(trains_vector);

    auto line = track.GetTrackId();

    if (line == types::TrackId::TRACKID_GREEN)
    {
        std::cout << std::endl << "Line color is " << "GREEN" << std::endl;
    }

    std::cout << std::endl << "Track contains " << trains_vector.size() << std::endl;

    trainptr->SetDistanceTraveled(110);

    //train on 64 and 63

    track.Update();

    auto occupancy = track.GetOccupiedTrainBlocks();

    std::cout << std::endl << "Train is occupying the following blocks: " << std::endl;

    for (size_t i=0;i<occupancy[0].size();i++)
    {
        std::cout << std::endl << occupancy[0][i] << std::endl;
    }

    track.SetAuthority(64, 10);
    track.SetCommandedSpeed(64, 500);

    auto trainAuth = trainptr->GetAuthority();
    auto trainCS = trainptr->GetCommandedSpeed();

    std::cout << std::endl << "Train authority is : " << trainAuth << std::endl;
    std::cout << std::endl << "Train commanded speed is : " << trainCS << std::endl;

    std::cout << std::endl << "There are currently " << trainptr->GetPassengersCount() << " passengers on board" << std::endl;

    trainptr->SetDistanceTraveled(290);

    track.Update();

    occupancy = track.GetOccupiedTrainBlocks();

    std::cout << std::endl << "Train is occupying the following blocks: " << std::endl;

    for (size_t i=0;i<occupancy[0].size();i++)
    {
        std::cout << std::endl << occupancy[0][i] << std::endl;
    }

    std::cout << std::endl << "There are currently " << trainptr->GetPassengersCount() << " passengers on board" << std::endl;

    // launcher_ui->on_launch_ctc_window([&]
    // {
    //     ctc_ui->show();
    // });
    // launcher_ui->on_launch_wayside_controller_window([&]
    // {
    //     wayside_controller_ui->show();
    // });
    // launcher_ui->on_launch_track_model_window([&]
    // {
    //     track_model_ui->show();
    // });
    // launcher_ui->on_launch_train_model_window([&]
    // {
    //     train_model_ui->show();
    // });
    // launcher_ui->on_launch_train_controller_window([&]
    // {
    //     train_controller_ui->show();
    // });

    // launcher_ui->run();

    return 0;
}