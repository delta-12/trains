#include <thread>

#include <slint.h>

#include "launcher.h"
#include "simulator.h"
#include "ctc.h"
#include "types.h"
#include "ctc_callback_handler.h"
#include "channel.h"

int main(void)
{
    simulator::Simulator world;
    auto                 launcher_ui           = ui::Launcher::create();
    auto                 ctc_ui                = ui::CtcUi::create();
    auto                 wayside_controller_ui = ui::WaysideControllerUi::create();
    auto                 track_model_ui        = ui::TrackModelUi::create();
    auto                 train_model_ui        = ui::TrainModelUi::create();
    auto                 train_controller_ui   = ui::TrainControllerUi::create();


    Channel<std::string> ctc_block_occupancy_channel;
    Channel<std::string> ctc_manual_dispatch_channel;
    Channel<std::vector<types::BlockState>> ctc_block_states_channel;
    // Setting Up CTC
    ctc::Ctc ctc(types::TrackId::TRACKID_GREEN);

    auto block_data_model = std::make_shared<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(); 
    std::vector<types::Block> blocks = ctc.GetBlocks(); 
    blocks.erase(blocks.begin());
    for (const types::Block &block : blocks) {
        auto block_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
        block_entry->push_back(slint::StandardListViewItem({text: std::string(1, block.section).c_str()}));
        block_entry->push_back(slint::StandardListViewItem({text: std::to_string(block.block).c_str()}));
        block_entry->push_back(slint::StandardListViewItem({text: block.maintenance ? "Maintenance" : "Open"}));
        block_entry->push_back(slint::StandardListViewItem({text: "_"}));
        block_entry->push_back(slint::StandardListViewItem({text: block.occupied ? "Occupied" : "_"}));
        block_entry->push_back(slint::StandardListViewItem({text: block.power_failure ? "Failure" : "_"}));
        block_data_model->push_back(block_entry);
    }
    ctc_ui->set_block_data(block_data_model);
    auto received_block_data = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ctc_ui->get_block_data());
    // Create Model to Populate Train Schedule Table
    auto train_schedule_model = std::make_shared<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>();
    ctc_ui->set_train_schedules(train_schedule_model);
    auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ctc_ui->get_train_schedules());
    
    // Create Model to Populate Trains Dropdown
    auto train_ids = std::make_shared<slint::VectorModel<slint::SharedString>>();
    train_ids->push_back(slint::SharedString("New Train"));
    ctc_ui->set_trains(train_ids);
    auto received_train_ids = std::dynamic_pointer_cast<slint::VectorModel<slint::SharedString>>(ctc_ui->get_trains());

    // Create Model to Populate Stations
    auto stations_model = std::make_shared<slint::VectorModel<slint::SharedString>>();
    std::vector<ctc::Station> stations = ctc.GetStations();
    for (const ctc::Station &station : stations) {
        stations_model->push_back(station.station_name.c_str());
    }
    ctc_ui->set_stations(stations_model);


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
    
    ctc_ui->on_manual_dispatch([&] {
        ctc_manual_dispatch_channel.Send(std::string(ctc_ui->get_destination()));
        ctc::handle_manual_dispatch(ctc_ui, ctc, ctc_manual_dispatch_channel);
    });

    ctc_ui->on_send_occupancy([&] {
        ctc_block_occupancy_channel.Send(std::string(ctc_ui->get_block_occupancy()));
        ctc::handle_set_occupancy(ctc_ui, ctc, received_train_schedules, received_block_data, ctc_block_occupancy_channel);
    });

    ctc_ui->on_wc_send_block_states([&] {
        std::vector<types::BlockState> block_states = { types::BlockState(63, true, false), types::BlockState(90, true, false), types::BlockState(91, true, false)};
        ctc_block_states_channel.Send(block_states);
        ctc::handle_set_block_states(ctc, received_train_schedules, received_block_data, ctc_block_states_channel);
    });

    // Test Integration



    std::thread worker_thread([&]
    {
        // Main backend loop here
    });

    launcher_ui->run();
    worker_thread.join();

    return 0;
}

