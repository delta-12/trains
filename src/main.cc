#include <memory>
#include <thread>
#include <iostream>
#include <map>
#include <slint.h>

#include "launcher.h"
#include "block_builder.h"
#include "csv_parser.h"
#include "simulator.h"
#include "tick_source.h"
#include "track_model.h"
#include "train_model.h"
#include "train_controller.h"

int main(void)
{

    simulator::Simulator world;
    std::map<int,std::shared_ptr<train_controller::SoftwareTrainController>> train_controllers;
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


    std::thread worker_thread([&world, &train_controllers]
    {
        std::shared_ptr<TickSource> tick_source = std::make_shared<TickSource>();
        tick_source.get()->Start();

        std::filesystem::path           base_path = std::filesystem::current_path();
        std::filesystem::path           path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_path.csv";
        std::filesystem::path           path2     = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_track_layout.csv";
        CsvParser                       parser(path);
        CsvParser                       parser2(path2);
        BlockBuilder                    bb(parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
        BlockBuilder                    bb2(parser2.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
        
        
        std::shared_ptr<train_model::TrainModel> train = std::make_shared<train_model::SoftwareTrainModel>(tick_source);
        std::shared_ptr<train_controller::SoftwareTrainController> train_contr = std::make_shared<train_controller::SoftwareTrainController>(tick_source);
        std::shared_ptr<track_model::SoftwareTrackModel> track = std::make_shared<track_model::SoftwareTrackModel>();


        train.get()->SetTrainId(0);
        train_controllers[0] = train_contr;
        track->SetTrackLayout(types::TrackId::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());
        world.AddTrackModel(track);
        world.AddTrainModel(track->GetTrackId(), train);


        while(1)
        {
            types::Meters dist = train_controllers[0].get()->GetDistanceTravelled();
            
            if(dist == 0)
            {
                track.get()->SetAuthority(0,7);
            }

            types::Watts commanded_power = train_controllers[0].get()->GetCommandedPower();
            types::Meters ditsance_traveled_since_last_update = train_controllers[0].get()->GetDistanceTravelledSinceLastUpdate();

            track.get()->GetTrainModel(0).get()->SetCommandedPower(commanded_power);
            track.get()->GetTrainModel(0).get()->SetDistanceTraveled(ditsance_traveled_since_last_update);


            world.Update();


            types::Polarity polarity = track.get()->GetTrainModel(0).get()->GetTrackPolarity();
            types::Blocks authority = track.get()->GetTrainModel(0).get()->GetAuthority();

            train_controllers[0].get()->SetPolartity(polarity);
            train_controllers[0].get()->SetAuthority(authority);

            train_controllers[0].get()->Update();
        }
    });


    //std::cout << x;
    launcher_ui->run();
    worker_thread.join();

    return 0;
}