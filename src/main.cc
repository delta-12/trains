#include <thread>

#include <slint.h>
#include "channel.h"
#include "wayside_controller.h"
#include "launcher.h"
#include "simulator.h"
#include "wayside_controller_callback_handler.h"


//void register_wayside_controller_callbacks(){}




int main(void)
{
    simulator::Simulator world;
    auto                 launcher_ui           = ui::Launcher::create();
    auto                 ctc_ui                = ui::CtcUi::create();
    auto                 wayside_controller_ui = ui::WaysideControllerUi::create();
    auto                 track_model_ui        = ui::TrackModelUi::create();
    auto                 train_model_ui        = ui::TrainModelUi::create();
    auto                 train_controller_ui   = ui::TrainControllerUi::create();



/// wc callbacks insts //
    wayside_controller::WaysideController wc;



    wayside_controller_ui->on_disp_sugg_auth([&]    
    {
        //getter function == take input from ui
        std::string auth_ui = std::string(wayside_controller_ui->get_sugg_auth());

        //make slint weak ui handle
        slint::ComponentWeakHandle<ui::WaysideControllerUi> weak_ui_handle(wayside_controller_ui);

        //create worker thread
        std::thread worker_thread([&]{

        //instantiate authority channel within worker thread
        Channel<std::string> wc_authority;

        //send ui input over channel
        wc_authority.Send(auth_ui);

        //invoke event handler
        slint::invoke_from_event_loop([weak_ui_handle, &wc_authority](){

            if (auto ui = weak_ui_handle.lock()){

                if(ui.has_value()){

                    //ui.value()->on_disp_sugg_auth(wc_authority.Receive());

                    //call CB handler funct here
                    ui.value()->get_comm_auth(const (wc_authority.Receive().c_str()));

                }
                
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));

        });




            
    });






    });


    ///



/// wc callbacks insts //

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
        // Main backend loop here
    });

    launcher_ui->run();
    worker_thread.join();

    return 0;
}

//functionality must occur on the worker thread