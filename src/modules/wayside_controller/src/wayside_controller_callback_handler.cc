#include "wayside_controller_callback_handler.h"

namespace wayside_controller{

    //TB authortu
    void disp_sugg_auth(slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui, wayside_controller::WaysideController &wc, Channel<std::string> &channel){


        if(channel.DataAvailable()){

            //channel recieve with ready data
           std::string auth = channel.Receive();

           //types::TrackCircuitData temp_data (types::TrackId::TRACKID_GREEN, 1, 0, std::stoi(auth));

           //wc.GetCommandedSpeedAndAuthority(temp_data);

           wayside_controller_ui->set_comm_auth(auth.c_str());
        }

    }









}