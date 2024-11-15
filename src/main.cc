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

        train_model_ui->on_engine_failure_change([&]{
            bool temp = train_model_ui->get_engine_fail();
            TM.SetEngineFailure(temp);
        });

        train_model_ui->on_brake_failure_change([&]{
            bool temp = train_model_ui->get_brake_fail();
            TM.SetBrakeFailure(temp);
        });

        train_model_ui->on_sig_pickup_failure_change([&]{
            bool temp = train_model_ui->get_signal_fail();
            TM.SetSignalPickupFailure(temp);
        });

        train_model_ui->on_ebrake_change([&]{
            bool temp = train_model_ui->get_ebrake();
            TM.SetEmergencyBrake(temp);
        });

        train_model_ui->on_update([&]{

            bool inputError = false;

            auto temp1a = std::string(train_model_ui->get_comm_speed_in());
            if (!temp1a.empty()) {
                try {
                    float temp1b = std::stof(temp1a);
                    if (temp1b < 0)
                    {
                        temp1b = 0;
                    }
                    TM.SetCommandedSpeed(temp1b);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            auto temp2a = std::string(train_model_ui->get_authority_in());
            if (!temp2a.empty()) {
                try {
                    float temp2b = std::stof(temp2a);
                    if (temp2b < 0)
                    {
                        temp2b = 0;
                    }
                    TM.SetAuthority(temp2b);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            auto temp3a = std::string(train_model_ui->get_pass_board_in());
            if (!temp3a.empty()) {
                try {
                    int temp3b = std::stoi(temp3a);
                    if (temp3b < 0)
                    {
                        temp3b = 0;
                    }
                    TM.SetPassengersBoarding(temp3b);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            auto temp4a = std::string(train_model_ui->get_left_door_in());
            if (!temp4a.empty()) {
                try {
                    bool temp4b = (temp4a == "true");
                    if (temp4b == 0)
                    {
                        temp4b = 0;
                    }
                    TM.SetLeftDoorsState(temp4b);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            auto temp5a = std::string(train_model_ui->get_right_door_in());
            if (!temp5a.empty()) {
                try {
                    bool temp5b = (temp5a == "true");
                    if (temp5b == 0)
                    {
                        temp5b = 0;
                    }
                    TM.SetRightDoorsState(temp5b);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            auto temp6a = std::string(train_model_ui->get_ext_light_in());
            if (!temp6a.empty()) {
                try {
                    bool temp6b = (temp6a == "true");
                    if (temp6b == 0)
                    {
                        temp6b = 0;
                    }
                    TM.SetHeadlights(temp6b);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            auto temp7a = std::string(train_model_ui->get_int_light_in());
            if (!temp7a.empty()) {
                try {
                    bool temp7b = (temp7a == "true");
                    if (temp7b == 0)
                    {
                        temp7b = 0;
                    }
                    TM.SetInternalLights(temp7b);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            auto temp8a = std::string(train_model_ui->get_comm_power_in());
            if (!temp8a.empty()) {
                try {
                    float temp8b = std::stof(temp8a);
                    if (temp8b < 0)
                    {
                        temp8b = 0;
                    }
                    TM.SetCommandedPower(temp8b);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            auto temp9a = std::string(train_model_ui->get_dist_trav_in());
            if (!temp9a.empty()) {
                try {
                    float temp9b = std::stof(temp9a);
                    if (temp9b < 0)
                    {
                        temp9b = 0;
                    }
                    TM.SetDistanceTraveled(temp9b);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            auto temp10a = std::string(train_model_ui->get_service_brake_in());
            if (!temp10a.empty()) {
                try {
                    float temp10b = std::stof(temp10a);
                    if (temp10b < 0)
                    {
                        temp10b = 0;
                    }
                    TM.SetBrake(temp10b);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            auto temp11a = std::string(train_model_ui->get_int_temp_in());
            if (!temp11a.empty()) {
                try {
                    int temp11b = std::stof(temp11a);
                    if (temp11b < 0)
                    {
                        temp11b = 0;
                    }
                    TM.SetCommandedInternalTemperature(temp11b);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            auto temp12a = std::string(train_model_ui->get_ebrake_in());
            if (!temp12a.empty()) {
                try {
                    bool temp12b = (temp12a == "true");
                    if (temp12b == 0)
                    {
                        temp12b = 0;
                    }
                    TM.SetEmergencyBrake(temp12b);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            auto temp13a = std::string(train_model_ui->get_station_announce_in());
            if (!temp13a.empty()) {
                try {
                    TM.SetStationAnnouncement(temp13a);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            if (inputError)
            {
                inputError = false;
            }

            TM.Update();

            //TM.SetTrackPolarity                (std::string(train_model_ui->get_track_pol_in()) == "true");

            // TM.SetStationAnnouncement          (std::string(train_model_ui->get_station_announce_in()));

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
            train_model_ui->set_brake_fail(TM.GetBrakeFailure());
            train_model_ui->set_engine_fail(TM.GetEngineFailure());
            train_model_ui->set_signal_fail(TM.GetSignalPickupFailure());
        });


    launcher_ui->run();
    worker_thread.join();

    return 0;
}