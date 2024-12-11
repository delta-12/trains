#include <memory>
#include <slint_string.h>
#include <thread>
#include <iostream>
#include <map>
#include <slint.h>
#include <slint_timer.h>
#include <unistd.h>

#include "common/inc/types.h"
#include "launcher.h"
#include "tick_source.h"
#include "train_controller.h"

int main(void)
{
    std::map<int,std::shared_ptr<train_controller::SoftwareTrainController>> train_controllers;

    auto launcher_ui           = ui::Launcher::create();
    auto ctc_ui                = ui::CtcUi::create();
    auto wayside_controller_ui = ui::WaysideControllerUi::create();
    auto track_model_ui        = ui::TrackModelUi::create();
    auto train_model_ui        = ui::TrainModelUi::create();
    auto train_controller_ui   = ui::TrainControllerUi::create();

    // Keep launchers - for future integration
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

    // Create a tick source and a single train controller
    std::shared_ptr<TickSource> tick_source = std::make_shared<TickSource>();
    tick_source->Start();
    tick_source->SetMultiplier(1);

    std::shared_ptr<train_controller::SoftwareTrainController> train_contr = 
        std::make_shared<train_controller::SoftwareTrainController>(tick_source);

    train_controllers[0] = train_contr;


    // Setup timer to periodically update UI elements from the train controller
    slint::Timer timer;
    timer.start(slint::TimerMode::Repeated, std::chrono::milliseconds(500), [=, &train_controllers, &train_controller_ui]() {
        // Update UI from the single train controller
        train_controller_ui->set_current_velocity(train_controllers[0]->GetCurrentSpeed());
        train_controller_ui->set_commanded_speed(train_controllers[0]->GetCommandedSpeed());
        train_controller_ui->set_authority(train_controllers[0]->GetAuthority());
        train_controller_ui->set_actual_internal_temperature(train_controllers[0]->GetActualInternalTemperature());
        train_controller_ui->set_distance_traveled(train_controllers[0]->GetDistanceTravelled());
        train_controller_ui->set_distance_traveled_since_last_update(train_controllers[0]->GetDistanceTravelledSinceLastUpdate());
        train_controller_ui->set_commanded_power(train_controllers[0]->GetCommandedPower());
        train_controller_ui->set_service_brake(train_controllers[0]->GetServiceBrake() * 100);
        train_controller_ui->set_int_lights(train_controllers[0]->GetInteriorLights());
        train_controller_ui->set_ext_lights(train_controllers[0]->GetHeadLights());
        train_controller_ui->set_left_doors(train_controllers[0]->GetLeftDoors());
        train_controller_ui->set_right_doors(train_controllers[0]->GetRightDoors());
        train_controller_ui->set_station_name(slint::SharedString(train_controllers[0]->GetCurrentStationName()));
        train_controller_ui->set_emergency_brake(train_controllers[0]->GetEmergencyBrake());
        train_controller_ui->set_kp(train_controllers[0]->GetKP());
        train_controller_ui->set_ki(train_controllers[0]->GetKI());
    });

    // TRAIN CONTROLLER CALLBACKS

    // Since we only have one train, this selection change does nothing significant now.
    train_controller_ui->on_request_train_selection_changed([&] {
        // Just always use train_controllers[0]
    });

    train_controller_ui->on_request_automatic_mode([&] {
        // If it's already manual, we cannot switch back to automatic
        if (train_controllers[0]->GetOperationMode() == true) {
            train_controller_ui->set_operation_status_message("In Manual, cannot switch to Automatic");
            auto tmp_timer = std::make_shared<slint::Timer>();
            tmp_timer->single_shot(std::chrono::seconds(3), [train_controller_ui, tmp_timer]() {
                train_controller_ui->set_operation_status_message("");
            });
        }
    });

    train_controller_ui->on_request_manual_mode([&] {
        if (train_controllers[0]->GetOperationMode() == false) {
            train_controllers[0]->SetOperationMode(true);
            train_controller_ui->set_operation_mode_status("Manual");
        }
    });

    // Update handler
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
                train_controllers[0]->SetCurrentSpeed(temp_current_velocity);
            } catch (const std::exception&) {
                inputError = true;
            }
        }

        // commanded speed
        auto temp_commanded_speed_str = std::string(train_controller_ui->get_temp_commanded_speed());
        if (!temp_commanded_speed_str.empty()) {
            try {
                float temp_commanded_speed = std::stof(temp_commanded_speed_str);
                train_controllers[0]->SetCommandedSpeed(temp_commanded_speed);
            } catch (const std::exception&) {
                inputError = true;
            }
        }

        // authority
        auto temp_authority_str = std::string(train_controller_ui->get_temp_authority());
        if (!temp_authority_str.empty()) {
            try {
                float temp_authority = std::stof(temp_authority_str);
                train_controllers[0]->SetAuthority(temp_authority);
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
                train_controllers[0]->SetActualInternalTemperature(temp_actual_internal_temperature);
            } catch (const std::exception&) {
                inputError = true;
            }
        }

        // polarity
        auto temp_polarity_str = std::string(train_controller_ui->get_temp_polarity());
        if (!temp_polarity_str.empty()) {
            if (temp_polarity_str == "+") {
                train_controllers[0]->SetPolartity(types::Polarity::POLARITY_POSITIVE);
            } else if (temp_polarity_str == "-") {
                train_controllers[0]->SetPolartity(types::Polarity::POLARITY_NEGATIVE);
            } else {
                inputError = true;
            }
        }

        if (inputError)
        {
            // We could show an error message, but for now just ignore
        }

        train_controllers[0]->Update();

        // Refresh UI after update
        train_controller_ui->set_current_velocity(train_controllers[0]->GetCurrentSpeed());
        train_controller_ui->set_commanded_speed(train_controllers[0]->GetCommandedSpeed());
        train_controller_ui->set_authority(train_controllers[0]->GetAuthority());
        train_controller_ui->set_actual_internal_temperature(train_controllers[0]->GetActualInternalTemperature());
        train_controller_ui->set_distance_traveled(train_controllers[0]->GetDistanceTravelled());
        train_controller_ui->set_distance_traveled_since_last_update(train_controllers[0]->GetDistanceTravelledSinceLastUpdate());
        train_controller_ui->set_commanded_power(train_controllers[0]->GetCommandedPower());
        train_controller_ui->set_service_brake(train_controllers[0]->GetServiceBrake() * 100);
        train_controller_ui->set_int_lights(train_controllers[0]->GetInteriorLights());
        train_controller_ui->set_ext_lights(train_controllers[0]->GetHeadLights());
        train_controller_ui->set_left_doors(train_controllers[0]->GetLeftDoors());
        train_controller_ui->set_right_doors(train_controllers[0]->GetRightDoors());
        train_controller_ui->set_station_name(slint::SharedString(train_controllers[0]->GetCurrentStationName()));
        train_controller_ui->set_emergency_brake(train_controllers[0]->GetEmergencyBrake());
        train_controller_ui->set_kp(train_controllers[0]->GetKP());
        train_controller_ui->set_ki(train_controllers[0]->GetKI());
    });

    // Failures
    train_controller_ui->on_request_engine_fail([&] {
        if (train_controllers[0]->GetEngineFailure() == 0) {
            train_controllers[0]->SetEngineFailure(1);
            train_controllers[0]->SetEmergencyBrake(1);
            train_controllers[0]->SetCommandedPower(0);
            train_controllers[0]->SetServiceBrake(0);
            train_controllers[0]->Update();
            train_controller_ui->set_engine_status(train_controllers[0]->GetEngineFailure());
            train_controller_ui->set_emergency_brake(train_controllers[0]->GetEmergencyBrake());
            train_controller_ui->set_commanded_power(train_controllers[0]->GetCommandedPower());
            train_controller_ui->set_service_brake(train_controllers[0]->GetServiceBrake() * 100);
        } else {
            train_controllers[0]->SetEngineFailure(0);
            train_controller_ui->set_engine_status(train_controllers[0]->GetEngineFailure());
            train_controllers[0]->Update();
            train_controller_ui->set_emergency_brake(train_controllers[0]->GetEmergencyBrake());
            train_controller_ui->set_commanded_power(train_controllers[0]->GetCommandedPower());
        }
    });

    train_controller_ui->on_request_brake_fail([&] {
        if (train_controllers[0]->GetBrakeFailure() == 0) {
            train_controllers[0]->SetBrakeFailure(1);
            train_controllers[0]->SetEmergencyBrake(1);
            train_controllers[0]->SetCommandedPower(0);
            train_controllers[0]->SetServiceBrake(0);
            train_controllers[0]->Update();
            train_controller_ui->set_brake_status(train_controllers[0]->GetBrakeFailure());
            train_controller_ui->set_emergency_brake(train_controllers[0]->GetEmergencyBrake());
            train_controller_ui->set_commanded_power(train_controllers[0]->GetCommandedPower());
            train_controller_ui->set_service_brake(train_controllers[0]->GetServiceBrake() * 100);
        } else {
            train_controllers[0]->SetBrakeFailure(0);
            train_controller_ui->set_brake_status(train_controllers[0]->GetBrakeFailure());
        }
    });

    train_controller_ui->on_request_signal_fail([&] {
        if (train_controllers[0]->GetSignalPickupFailure() == 0) {
            train_controllers[0]->SetSignalPickupFailure(1);
            train_controllers[0]->SetEmergencyBrake(1);
            train_controllers[0]->SetCommandedPower(0);
            train_controllers[0]->SetServiceBrake(0);
            train_controllers[0]->Update();
            train_controller_ui->set_signal_status(train_controllers[0]->GetSignalPickupFailure());
            train_controller_ui->set_emergency_brake(train_controllers[0]->GetEmergencyBrake());
            train_controller_ui->set_commanded_power(train_controllers[0]->GetCommandedPower());
            train_controller_ui->set_service_brake(train_controllers[0]->GetServiceBrake() * 100);
        } else {
            train_controllers[0]->SetSignalPickupFailure(0);
            train_controller_ui->set_signal_status(train_controllers[0]->GetSignalPickupFailure());
        }
    });

    // Manual mode inputs
    train_controller_ui->on_request_update_service_brake([&] {
        if (train_controllers[0]->GetOperationMode() == 1)
        {
            float temp = std::stof(std::string(train_controller_ui->get_temp_service_brake()));
            train_controllers[0]->SetServiceBrake(temp);
            train_controllers[0]->Update();
            train_controller_ui->set_service_brake(train_controllers[0]->GetServiceBrake() * 100);
            train_controller_ui->set_commanded_power(train_controllers[0]->GetCommandedPower());
            train_controller_ui->set_distance_traveled(train_controllers[0]->GetDistanceTravelled());
        }
    });

    train_controller_ui->on_request_update_commanded_internal_temperature([&] {
        if (train_controllers[0]->GetOperationMode() == 1) {
            float temp = std::stof(std::string(train_controller_ui->get_temp_commanded_internal_temperature()));
            if (temp < 65) {
                temp = 65;
            } else if (temp > 75) {
                temp = 75;
            }
            train_controllers[0]->SetCommandedInternalTemperature(temp);
            train_controller_ui->set_commanded_internal_temperature(train_controllers[0]->GetCommandedInternalTemperature());
        }
    });

    train_controller_ui->on_request_update_driver_speed([&] {
        if (train_controllers[0]->GetOperationMode() == 1) 
        {
            float temp = std::stof(std::string(train_controller_ui->get_temp_driver_speed()));
            train_controllers[0]->SetDriverSpeed(temp);
            train_controllers[0]->Update();
            train_controller_ui->set_driver_speed(train_controllers[0]->GetDriverSpeed());
            train_controller_ui->set_commanded_power(train_controllers[0]->GetCommandedPower());
            train_controller_ui->set_service_brake(train_controllers[0]->GetServiceBrake() * 100);
            train_controller_ui->set_distance_traveled(train_controllers[0]->GetDistanceTravelled());
            train_controller_ui->set_distance_traveled_since_last_update(train_controllers[0]->GetDistanceTravelledSinceLastUpdate());
        }
    });

    train_controller_ui->on_request_toggle_int_lights([&] {
        if (train_controllers[0]->GetOperationMode() == 1 && !train_controllers[0]->IsAtStation()) {
            bool current_state = train_controllers[0]->GetInteriorLights();
            train_controllers[0]->SetInteriorLights(!current_state);
            train_controller_ui->set_int_lights(train_controllers[0]->GetInteriorLights());
        }
    });

    train_controller_ui->on_request_toggle_ext_lights([&] {
        if (train_controllers[0]->GetOperationMode() == 1 && !train_controllers[0]->IsUnderground()) {
            bool current_state = train_controllers[0]->GetHeadLights();
            train_controllers[0]->SetHeadLights(!current_state);
            train_controller_ui->set_ext_lights(train_controllers[0]->GetHeadLights());
        } else if (train_controllers[0]->IsUnderground()) {
            train_controllers[0]->SetHeadLights(true);
            train_controller_ui->set_ext_lights(true);
        }
    });

    train_controller_ui->on_request_toggle_left_doors([&] {
        if (train_controllers[0]->GetOperationMode() == 1 && train_controllers[0]->CanOpenDoors()) {
            bool current_state = train_controllers[0]->GetLeftDoors();
            if (current_state) {
                train_controllers[0]->SetLeftDoors(false);
                train_controller_ui->set_left_doors(false);
            }
        }
    });

    train_controller_ui->on_request_toggle_right_doors([&] {
        if (train_controllers[0]->GetOperationMode() == 1 && train_controllers[0]->CanOpenDoors()) {
            bool current_state = train_controllers[0]->GetRightDoors();
            if (current_state) {
                train_controllers[0]->SetRightDoors(false);
                train_controller_ui->set_right_doors(false);
            }
        }
    });

    train_controller_ui->on_request_toggle_emergency_brake([&] {
        bool current_state = train_controllers[0]->GetEmergencyBrake();
        train_controllers[0]->SetEmergencyBrake(!current_state);
        train_controller_ui->set_emergency_brake(!current_state);
        train_controllers[0]->Update();
        train_controller_ui->set_service_brake(train_controllers[0]->GetEmergencyBrake());
    });

    train_controller_ui->on_request_update_kp_ki([&] {
        if (train_controllers[0]->GetDistanceTravelled() == 0)
        {
            auto temp_kp_str = std::string(train_controller_ui->get_temp_kp());
            auto temp_ki_str = std::string(train_controller_ui->get_temp_ki());

            bool inputError = false;

            if (!temp_kp_str.empty()) {
                try {
                    uint16_t temp_kp = static_cast<uint16_t>(std::stoul(temp_kp_str));
                    train_controllers[0]->SetKP(temp_kp);
                    train_controller_ui->set_kp(temp_kp);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            if (!temp_ki_str.empty()) {
                try {
                    uint16_t temp_ki = static_cast<uint16_t>(std::stoul(temp_ki_str));
                    train_controllers[0]->SetKI(temp_ki);
                    train_controller_ui->set_ki(temp_ki);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            if (inputError) {
                std::cout << "Invalid Kp or Ki value entered." << std::endl;
            }
        } else {
            std::cout << "Cannot update Kp and Ki after the train has been dispatched." << std::endl;
        }
    });

    train_controller_ui->on_request_reset_kp_ki([&] {
        if (train_controllers[0]->GetDistanceTravelled() == 0)
        {
            train_controllers[0]->SetKP(5000);
            train_controllers[0]->SetKI(100);
            train_controller_ui->set_kp(5000);
            train_controller_ui->set_ki(100);
            train_controller_ui->set_temp_kp("");
            train_controller_ui->set_temp_ki("");
        } else {
            std::cout << "Cannot reset Kp and Ki after the train has been dispatched." << std::endl;
        }
    });

    launcher_ui->run();

    timer.stop();
    return 0;
}
