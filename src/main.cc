#include <thread>

#include <filesystem>

#include <slint.h>
#include "channel.h"
#include "wayside_controller.h"
#include "launcher.h"
#include "simulator.h"
#include "types.h"

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

    
    std::function<wayside_controller::Error(const wayside_controller::InputId input, wayside_controller::IoSignal &signal)> get_input_;



    // wayside block initialization //

    wayside_controller::WaysideBlock wb25(25, 26, 26, types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL, 25, 25, false, false, wayside_controller::IoSignal::IOSIGNAL_HIGH);
    wayside_controller::WaysideBlock wb26(26, 27, 27, types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL, 26, 26, false, false, wayside_controller::IoSignal::IOSIGNAL_HIGH);
    wayside_controller::WaysideBlock wb27(27, 28, 28, types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL, 27, 27, false, false, wayside_controller::IoSignal::IOSIGNAL_HIGH);

    //block 28 on green line has the switch
    wayside_controller::WaysideBlock wb28(28, 29, 150, types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL, 28, 28, true, false, wayside_controller::IoSignal::IOSIGNAL_HIGH);


    std::vector<wayside_controller::WaysideBlock> blocks_stub;

    blocks_stub.push_back(wb25);
    blocks_stub.push_back(wb26);
    blocks_stub.push_back(wb27);
    blocks_stub.push_back(wb28);
    
    // wayside block initialization //


    // wayside controller initialization //

    wayside_controller::WaysideController wc(get_input_, blocks_stub);

    // wayside controller initialization //

    
    
    slint::ComponentWeakHandle<ui::WaysideControllerUi> weak_ui_handle(wayside_controller_ui);




    //callbacks config


    //load plc file
    wayside_controller_ui->on_parse_file([&] (){

    // choose and parse file here
    //std::cout << "Parsing File..." << std::endl;




    });


    //disp authority from ctc
    wayside_controller_ui->on_disp_sugg_auth([&] (){

        std::string auth_ui = std::string(wayside_controller_ui->get_sugg_auth());

        //temp int stuff
        // int auth_int = std::stoi(auth_ui);

        // auth_int = auth_int + 2;

        // std::string auth_str = std::to_string(auth_int);

        // wayside_controller_ui->set_comm_auth(auth_str.c_str());


// proper impl for backend
        types::TrackCircuitData temp_data (types::TrackId::TRACKID_GREEN, 1, 5, std::stoi(auth_ui));

        std::string result = (std::to_string(wc.GetCommandedAuthority(temp_data)));

        //call CB handler funct here
        wayside_controller_ui->set_comm_auth(result.c_str());


    });

    //disp speed from ctc
    wayside_controller_ui->on_disp_sugg_spd([&] (){

        std::string spd_ui = std::string(wayside_controller_ui->get_sugg_spd());

// proper impl for backend
        types::TrackCircuitData temp_data (types::TrackId::TRACKID_GREEN, 1, 5, std::stoi(spd_ui));

        std::string result = (std::to_string(wc.GetCommandedSpeed(temp_data)));

        //call CB handler funct here
        wayside_controller_ui->set_comm_spd(result.c_str());


    });


    wayside_controller_ui->on_disp_sw_state([&] (){

        std::string sw_state = std::string(wayside_controller_ui->get_str_sw_statein());

    // proper impl for backend 
        bool switch_data; 
        std::string result = "No Data Available";


        if(sw_state == "Open" || sw_state == "open"){

            switch_data = true;
        }
        else if(sw_state == "Closed" || sw_state == "closed"){

            switch_data = false;
        }

        wayside_controller::Error output = wc.SetSwitch(28, switch_data);


        if(output != wayside_controller::Error::ERROR_NONE){

            std::string result = "Invalid Switch State";

        }
        else{

            if(switch_data == true){
                result = "Closed";
            }
            if(switch_data == false){
                result = "Open";
            }
        }

        //call CB handler funct here
        wayside_controller_ui->set_str_sw_stateout(result.c_str());


    });


    wayside_controller_ui->on_disp_block_main([&] (){


    // proper impl for backend 
        bool switch_data; 
        std::string result = "No Data Available";

        std::string sw_state = std::string(wayside_controller_ui->get_str_block_mainin());



        if(sw_state == "Open" || sw_state == "open"){

            switch_data = true;
            std::string result = "Closed";

        }
        else if(sw_state == "Closed" || sw_state == "closed"){

            switch_data = false;
            std::string result = "Open";

        }

        wayside_controller::Error output = wc.SetSwitch(28, switch_data);


        if(output != wayside_controller::Error::ERROR_NONE){

            std::string result = "Invalid Switch State";
            

        }
        else{

            if(switch_data == true){
                result = "Closed";
            }
            if(switch_data == false){
                result = "Open";
            }
        }
        
        //call CB handler funct here
        wayside_controller_ui->set_str_block_mainout(result.c_str());

        });


    wayside_controller_ui->on_disp_block_occ([&] (){

        std::string result;

        std::string sw_state = std::string(wayside_controller_ui->get_str_block_occin());

        if(sw_state == "Occupied" || sw_state == "occupied"){

            result = "Occupied";

        }
        else if(sw_state == "Empty" || sw_state == "empty"){

            result = "Unoccupied";

        }

        wayside_controller_ui->set_str_block_occ(result.c_str());



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