#include <thread>

#include <slint.h>

#include "common/inc/types.h"
#include "launcher.h"
#include "simulator.h"
#include "train_controller.h"

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

    // TRAIN CONTROLLER CALLBACKS START

    TickSource                                tick_source("07:00:00", std::chrono::milliseconds(1));
    std::shared_ptr<TickSource>               CLOCK = std::make_shared<TickSource>(tick_source);
    train_controller::SoftwareTrainController TC(CLOCK);
    (*CLOCK).Start();

    // automatic mode
    train_controller_ui->on_request_automatic_mode([&] {
        if (TC.GetOperationMode() == 1) {
            train_controller_ui->set_operation_status_message("In Manual, cannot switch to Automatic");

            // timer to clear the message after 3 seconds
            auto timer = std::make_shared<slint::Timer>();
            timer->single_shot(std::chrono::seconds(3), [train_controller_ui, timer]() {
                train_controller_ui->set_operation_status_message("");
            });
        }
    });

    // manual mode
    train_controller_ui->on_request_manual_mode([&] {
        if (TC.GetOperationMode() == 0) {
            TC.SetOperationMode(1);
            train_controller_ui->set_operation_mode_status("Manual");
        }
    });

    // Update - Start

    train_controller_ui->on_request_update([&] {
       
       bool inputError = false;

        // current speed
        auto temp_current_velocity_str = std::string(train_controller_ui->get_temp_current_velocity());
        if (!temp_current_velocity_str.empty()) {
            try {
                float temp_current_velocity = std::stof(temp_current_velocity_str);
                if (temp_current_velocity < 0)
                {
                    temp_current_velocity = 0;
                }
                TC.SetCurrentSpeed(temp_current_velocity);
            } catch (const std::exception&) {
                inputError = true;
            }
        }

        // commanded speed
        auto temp_commanded_speed_str = std::string(train_controller_ui->get_temp_commanded_speed());
        if (!temp_commanded_speed_str.empty()) {
            try {
                float temp_commanded_speed = std::stof(temp_commanded_speed_str);
                TC.SetCommandedSpeed(temp_commanded_speed);
            } catch (const std::exception&) {
                inputError = true;
            }
        }

        // authority
        auto temp_authority_str = std::string(train_controller_ui->get_temp_authority());
        if (!temp_authority_str.empty()) {
            try {
                float temp_authority = std::stof(temp_authority_str);
                TC.SetAuthority(temp_authority);
            } catch (const std::exception&) {
                inputError = true;
            }
        }

        // actual internal temperature
        auto temp_actual_internal_temperature_str = std::string(train_controller_ui->get_temp_actual_internal_temperature());
        if (!temp_actual_internal_temperature_str.empty()) {
            try {
                float temp_actual_internal_temperature = std::stof(temp_actual_internal_temperature_str);
                if (temp_actual_internal_temperature < 65) {
                    temp_actual_internal_temperature = 65;
                } else if (temp_actual_internal_temperature > 75) {
                    temp_actual_internal_temperature = 75;
                }
                TC.SetActualInternalTemperature(temp_actual_internal_temperature);
            } catch (const std::exception&) {
                inputError = true;
            }
        }

        if (inputError)
        {
            inputError = false;
        }

        // call update
         TC.Update();

        // assign all UI elements
        train_controller_ui->set_current_velocity(TC.GetCurrentSpeed());
        train_controller_ui->set_commanded_speed(TC.GetCommandedSpeed());
        train_controller_ui->set_authority(TC.GetAuthority());
        train_controller_ui->set_actual_internal_temperature(TC.GetActualInternalTemperature());
        train_controller_ui->set_distance_traveled(TC.GetDistanceTravelled());
        train_controller_ui->set_distance_traveled_since_last_update(TC.GetDistanceTravelledSinceLastUpdate());
        train_controller_ui->set_commanded_power(TC.GetCommandedPower());
        train_controller_ui->set_service_brake(TC.GetServiceBrake() * 100);
    });

    // Update - End

    // Failures - Start

    // engine failure
    train_controller_ui->on_request_engine_fail([&] {
        if (TC.GetEngineFailure() == 0) {
            TC.SetEngineFailure(1);
            TC.SetEmergencyBrake(1);
            TC.SetCommandedPower(0);
            TC.SetServiceBrake(0);
            train_controller_ui->set_engine_status(TC.GetEngineFailure());
            train_controller_ui->set_emergency_brake(TC.GetEmergencyBrake());
            train_controller_ui->set_commanded_power(TC.GetCommandedPower());
            train_controller_ui->set_service_brake(TC.GetServiceBrake() * 100);
        } else {
            TC.SetEngineFailure(0);
            train_controller_ui->set_engine_status(TC.GetEngineFailure());
            TC.Update();
            train_controller_ui->set_emergency_brake(TC.GetEmergencyBrake());
            train_controller_ui->set_commanded_power(TC.GetCommandedPower());
        }
    });

    // brake failure
    train_controller_ui->on_request_brake_fail([&] {
        if (TC.GetBrakeFailure() == 0) {
            TC.SetBrakeFailure(1);
            TC.SetEmergencyBrake(1);
            TC.SetCommandedPower(0);
            TC.SetServiceBrake(0);
            train_controller_ui->set_brake_status(TC.GetBrakeFailure());
            train_controller_ui->set_emergency_brake(TC.GetEmergencyBrake());
            train_controller_ui->set_commanded_power(TC.GetCommandedPower());
            train_controller_ui->set_service_brake(TC.GetServiceBrake() * 100);
        } else {
            TC.SetBrakeFailure(0);
            train_controller_ui->set_brake_status(TC.GetBrakeFailure());
        }
    });

    // signal pickup failure
    train_controller_ui->on_request_signal_fail([&] {
        if (TC.GetSignalPickupFailure() == 0) {
            TC.SetSignalPickupFailure(1);
            TC.SetEmergencyBrake(1);
            TC.SetCommandedPower(0);
            TC.SetServiceBrake(0);
            train_controller_ui->set_signal_status(TC.GetSignalPickupFailure());
            train_controller_ui->set_emergency_brake(TC.GetEmergencyBrake());
            train_controller_ui->set_commanded_power(TC.GetCommandedPower());
            train_controller_ui->set_service_brake(TC.GetServiceBrake() * 100);
        } else {
            TC.SetSignalPickupFailure(0);
            train_controller_ui->set_signal_status(TC.GetSignalPickupFailure());
        }
    });

    // Failures - End

    // Manual - UI input parameters - Start

    // service brake input
    train_controller_ui->on_request_update_service_brake([&] {
        if (TC.GetOperationMode() == 1)
        {
            float temp = std::stof(std::string(train_controller_ui->get_temp_service_brake()));
            TC.SetServiceBrake(temp);
            //TC.SetCommandedPower(0);
            TC.Update();
            train_controller_ui->set_service_brake(TC.GetServiceBrake() * 100);
            train_controller_ui->set_commanded_power(TC.GetCommandedPower());
            train_controller_ui->set_distance_traveled(TC.GetDistanceTravelled());
        }
    });

    // commanded internal temperature
    train_controller_ui->on_request_update_commanded_internal_temperature([&] {
        if (TC.GetOperationMode() == 1) {
            float temp = std::stof(std::string(train_controller_ui->get_temp_commanded_internal_temperature()));
            if (temp < 65) {
                temp = 65;
            } else if (temp > 75) {
                temp = 75;
            }
            TC.SetCommandedInternalTemperature(temp);
            train_controller_ui->set_commanded_internal_temperature(TC.GetCommandedInternalTemperature());
        }
    });

    // driver speed
    train_controller_ui->on_request_update_driver_speed([&] {
        if (TC.GetOperationMode() == 1) 
        {
            float temp = std::stof(std::string(train_controller_ui->get_temp_driver_speed()));
            TC.SetDriverSpeed(temp);
            TC.Update();
            train_controller_ui->set_driver_speed(TC.GetDriverSpeed());
            train_controller_ui->set_commanded_power(TC.GetCommandedPower());
            train_controller_ui->set_service_brake(TC.GetServiceBrake() * 100);
            train_controller_ui->set_distance_traveled(TC.GetDistanceTravelled());
            train_controller_ui->set_distance_traveled_since_last_update(TC.GetDistanceTravelledSinceLastUpdate());
        }
    });

    // TRAIN CONTROLLER CALLBACKS END

    launcher_ui->run();
    worker_thread.join();

    return 0;
}