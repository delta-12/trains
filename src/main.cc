#include <thread>

#include <filesystem>

#include <slint.h>
#include "channel.h"
#include "wayside_controller.h"
#include "launcher.h"
#include "simulator.h"

//#include "wayside_controller_callback_handler.h"

// Prevent worker_thread keep running after application window is closed
//std::atomic<bool> keep_running(true);


//hotfix ui

int main(void)
{
    simulator::Simulator world;
    auto                 launcher_ui           = ui::Launcher::create();
    auto                 ctc_ui                = ui::CtcUi::create();
    auto                 wayside_controller_ui = ui::WaysideControllerUi::create();
    auto                 track_model_ui        = ui::TrackModelUi::create();
    auto                 train_model_ui        = ui::TrainModelUi::create();
    auto                 train_controller_ui   = ui::TrainControllerUi::create();

    
    
    slint::ComponentWeakHandle<ui::WaysideControllerUi> weak_ui_handle(wayside_controller_ui);

    //linker error????
    //wayside_controller::WaysideController wc;


    std::string input_plc_path="";

    std::filesystem::path base_path = std::filesystem::current_path();


    //callbacks config


    //load plc file
    wayside_controller_ui->on_parse_file([&] (){

    // choose and parse file here
    //std::cout << "Parsing File..." << std::endl;




    });

    wayside_controller_ui->on_disp_sugg_auth([&] (){

        std::string auth_ui = std::string(wayside_controller_ui->get_sugg_auth());

        //temp int stuff
        int auth_int = std::stoi(auth_ui);

        auth_int = auth_int + 2;

        std::string auth_str = std::to_string(auth_int);

        wayside_controller_ui->set_comm_auth(auth_str.c_str());


// proper impl for backend
        //types::TrackCircuitData temp_data (types::TrackId::TRACKID_GREEN, 1, 0, std::stoi(auth_ui));

        //std::string result = (std::to_string(wc.GetCommandedAuthority(temp_data)));

        //call CB handler funct here
        //wayside_controller_ui->set_comm_auth(result.c_str());


    });



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


    });

    launcher_ui->run();
    //tick_thread.join();
    worker_thread.join();

    return 0;

}






/*

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

    wayside_controller::setup_ui(wayside_controller_ui);




    std::thread worker_thread([&]
    {
        // Main backend loop here
        {   
            while (keep_running.load())
            {
                wayside_controller::backend_handler(wc, ctc_ui);
            }
        }

    });

    launcher_ui->run();
    //tick_thread.join();
    worker_thread.join();

    return 0;
}

*/

//functionality must occur on the worker thread