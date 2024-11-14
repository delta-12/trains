#include <thread>

#include <slint.h>
#include <unistd.h>
#include <cmath>
#include <iostream>
#include <iomanip>

#include "launcher.h"
#include "simulator.h"

#include "train_model.h"

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


    std::thread worker_thread([&]
    {
        // Main backend loop here
    });

    //TRAIN MODEL CALLBACKS

        TickSource                      tick_source("07:00:00", std::chrono::milliseconds(1));
        std::shared_ptr<TickSource>     CLOCK = std::make_shared<TickSource>(tick_source);
        train_model::SoftwareTrainModel TM(CLOCK);

        (*CLOCK).Start();

        train_model_ui->on_update([&]{
            
            TM.Update();

            TM.SetCommandedSpeed               (std::stof(std::string(train_model_ui->get_comm_speed_in())));
            TM.SetAuthority                    (std::stof(std::string(train_model_ui->get_authority_in())));
            TM.SetPassengersBoarding           (std::stoi(std::string(train_model_ui->get_pass_board_in())));
            //TM.SetTrackPolarity                (std::string(train_model_ui->get_track_pol_in()) == "true");
            TM.SetLeftDoorsState               (std::string(train_model_ui->get_left_door_in()) == "true");
            TM.SetRightDoorsState              (std::string(train_model_ui->get_right_door_in()) == "true");
            TM.SetHeadlights                   (std::string(train_model_ui->get_ext_light_in()) == "true");
            TM.SetInternalLights               (std::string(train_model_ui->get_int_light_in()) == "true");
            TM.SetCommandedPower               (std::stof(std::string(train_model_ui->get_comm_power_in())));
            TM.SetDistanceTraveled             (std::stof(std::string(train_model_ui->get_dist_trav_in())));
            TM.SetCommandedInternalTemperature (std::stoi(std::string(train_model_ui->get_int_temp_in())));
            TM.SetBrake                        (std::stof(std::string(train_model_ui->get_service_brake_in())));
            TM.SetEmergencyBrake               (std::string(train_model_ui->get_ebrake_in()) == "true");
            TM.SetStationAnnouncement          (std::string(train_model_ui->get_station_announce_in()));

            train_model_ui->set_comm_speed(TM.GetCommandedSpeed());
            train_model_ui->set_authority(TM.GetAuthority());
            train_model_ui->set_left_doors(TM.GetLeftDoorsState());
            train_model_ui->set_right_doors(TM.GetRightDoorsState());
            train_model_ui->set_ext_lights(TM.GetHeadlights());
            train_model_ui->set_int_lights(TM.GetInternalLights());
            train_model_ui->set_actual_power(TM.GetActualPower());
            train_model_ui->set_act_speed(TM.GetActualSpeed());
            train_model_ui->set_cabin_temp(TM.GetActualInternalTemperature());
            train_model_ui->set_service_brake(TM.GetBrake());
            train_model_ui->set_ebrake(TM.GetEmergencyBrake());
            train_model_ui->set_station_banner(slint::SharedString(TM.GetStationAnnouncement()));
            train_model_ui->set_time(slint::SharedString(tick_source.GetTimeString()));
            train_model_ui->set_acceleration(TM.GetAcceleration());
            train_model_ui->set_train_mass(TM.GetMass());
            train_model_ui->set_pass_count(TM.GetPassengersCount());

        });


    launcher_ui->run();
    worker_thread.join();

    return 0;
}