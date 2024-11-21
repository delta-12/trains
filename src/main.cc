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

    // TODO move these callbacks to ctc_callback_handler.cc
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

    // Setting Up CTC
    ctc::Ctc ctc_office(types::TrackId::TRACKID_GREEN);
    ctc::setup_ui(ctc_ui);

    std::thread worker_thread([&]
    {
<<<<<<< HEAD
        // Main backend loop here
=======
        while (true)
        {
            ctc::backend_handler(ctc_office);
        }
>>>>>>> ab4efb54101c6966f907255d0dee5a60752cd25d
    });

    launcher_ui->run();
    worker_thread.join();

    return 0;
}    
