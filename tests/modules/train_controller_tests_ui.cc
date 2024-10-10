/*****************************************************************************
 * @file train_controller_tests.cc
 *
 * @brief Short-term testing for the Train Controller backend.
 *****************************************************************************/

#include "train_controller.h"
#include "train_controller_ui.h"
#include <chrono>
#include <memory>
#include <slint_timer.h>
#include <string>
#include <thread>

int main(void)
{
    auto train_controller_ui = ui::TrainControllerUi::create();

    float IntegralSUM = 0;
    float maxPower = 120000;


    // Operation mode


    train_controller_ui->on_request_automatic_mode([&]{
        if (train_controller_ui->get_operation_mode_status() == "Manual")
        {
            train_controller_ui->set_operation_status_message("In Manual, cannot switch to Automatic");

            // timer to clear the message after 3 seconds
            auto timer = std::make_shared<slint::Timer>();
            timer->single_shot(std::chrono::seconds(3), [train_controller_ui, timer]() {
                train_controller_ui->set_operation_status_message("");
            });
        }
    });
    train_controller_ui->on_request_manual_mode([&]{
        train_controller_ui->set_operation_mode_status("Manual");
        });


    // Failures


    train_controller_ui->on_request_engine_fail([&] {
        if (train_controller_ui->get_engine_status() == 0) {
            train_controller_ui->set_engine_status(1);
            train_controller_ui->set_emergency_brake(1);
            train_controller_ui->set_commanded_power(0);
        }
        else
            train_controller_ui->set_engine_status(0);
    });
    train_controller_ui->on_request_brake_fail([&] {
        if (train_controller_ui->get_brake_status() == 0){
            train_controller_ui->set_brake_status(1);
            train_controller_ui->set_emergency_brake(1);
            train_controller_ui->set_commanded_power(0);
            train_controller_ui->set_service_brake(0);
        }
        else
            train_controller_ui->set_brake_status(0);
    });
    train_controller_ui->on_request_signal_fail([&] {
        if (train_controller_ui->get_signal_status() == 0) {
            train_controller_ui->set_signal_status(1);
            train_controller_ui->set_emergency_brake(1);
            train_controller_ui->set_commanded_power(0);
        }  
        else
            train_controller_ui->set_signal_status(0);
    });


    // Test Bench input parameters


    // actual internal temperature input from test bench
    train_controller_ui->on_request_update_actual_internal_temperature([&]{
        float temp = std::stof(std::string(train_controller_ui->get_temp_actual_internal_temperature()));
        if (temp < 65) {
            temp = 65;
        } else if (temp > 75) {
            temp = 75;
        }
            train_controller_ui->set_actual_internal_temperature(temp);
    });

    // commanded speed input from test bench
    train_controller_ui->on_request_update_commanded_speed([&]{
        float temp = std::stof(std::string(train_controller_ui->get_temp_commanded_speed()));
            train_controller_ui->set_commanded_speed(temp);
    });

    // authority from UI
    train_controller_ui->on_request_update_authority([&]{
        float temp = std::stof(std::string(train_controller_ui->get_temp_authority()));
            train_controller_ui->set_authority(temp);
    });


    // Manual - UI input parameters


    // service brake input from UI
    train_controller_ui->on_request_update_service_brake([&] {
        float temp = std::stof(std::string(train_controller_ui->get_temp_service_brake()));
            train_controller_ui->set_service_brake(temp);
            train_controller_ui->set_commanded_power(0);
            IntegralSUM = 0;
    });

    // commanded internal temperature from UI
    train_controller_ui->on_request_update_commanded_internal_temperature([&] {
        float temp = std::stof(std::string(train_controller_ui->get_temp_commanded_internal_temperature()));
        if (temp < 65) {
            temp = 65;
        } else if (temp > 75) {
            temp = 75;
        }
            train_controller_ui->set_commanded_internal_temperature(temp);
    });

    // update driver speed from UI
    train_controller_ui->on_request_update_driver_speed([&] {
        float temp = std::stof(std::string(train_controller_ui->get_temp_driver_speed()));
            train_controller_ui->set_driver_speed(temp);

        float blockSpeedLimitMph = 50.0; // default speed limit
        float current_velocity_mph = train_controller_ui->get_current_velocity();
        float driver_speed_mph = train_controller_ui->get_driver_speed();

        // Enforce block speed limit
        if (current_velocity_mph > blockSpeedLimitMph) {
            current_velocity_mph = blockSpeedLimitMph;
            train_controller_ui->set_current_velocity(current_velocity_mph);
        }

        // Convert speeds to m/s
        float currentVelocityInMPS = current_velocity_mph * 0.44704;
        float driverSpeedInMPS = driver_speed_mph * 0.44704;
        float blockSpeedLimitMPS = blockSpeedLimitMph * 0.44704;

        // Enforce speed limit on driver speed
        float Vsetpoint = driverSpeedInMPS;
        if (Vsetpoint > blockSpeedLimitMPS) {
            Vsetpoint = blockSpeedLimitMPS;
        }

        // Time delta
        float deltaTime = 1;

        if(train_controller_ui->get_emergency_brake() == true || train_controller_ui->get_authority() == 475)
        {
            IntegralSUM = 0;
            train_controller_ui->set_commanded_power(0);
        }
        else
        {
            if (currentVelocityInMPS > Vsetpoint) {
                IntegralSUM = 0;
                train_controller_ui->set_commanded_power(0);

                float tempSpeedDiffMPS = currentVelocityInMPS - Vsetpoint;
                float tempSpeedDiffMPH = tempSpeedDiffMPS / 0.44704;

                // Implement service brake increments based on speed difference
                int brakeLevel = 0;
                if(tempSpeedDiffMPH > 0 && tempSpeedDiffMPH <= 4.35)
                    brakeLevel = 10;
                else if(tempSpeedDiffMPH > 4.35 && tempSpeedDiffMPH <= 8.7)
                    brakeLevel = 20;
                else if(tempSpeedDiffMPH > 8.7 && tempSpeedDiffMPH <= 13.05)
                    brakeLevel = 30;
                else if(tempSpeedDiffMPH > 13.05 && tempSpeedDiffMPH <= 17.4)
                    brakeLevel = 40;
                else if(tempSpeedDiffMPH > 17.4 && tempSpeedDiffMPH <= 21.75)
                    brakeLevel = 50;
                else if(tempSpeedDiffMPH > 21.75 && tempSpeedDiffMPH <= 26.1)
                    brakeLevel = 60;
                else if(tempSpeedDiffMPH > 26.1 && tempSpeedDiffMPH <= 30.45)
                    brakeLevel = 70;
                else if(tempSpeedDiffMPH > 30.45 && tempSpeedDiffMPH <= 34.8)
                    brakeLevel = 80;
                else if(tempSpeedDiffMPH > 34.8 && tempSpeedDiffMPH <= 39.15)
                    brakeLevel = 90;
                else if(tempSpeedDiffMPH > 39.15)
                    brakeLevel = 100;

                train_controller_ui->set_service_brake(brakeLevel);

            } else {
                /*if (train_controller_ui->get_service_brake() > 0) {
                    train_controller_ui->set_service_brake(0);
                }*/
                train_controller_ui->set_service_brake(0);

                float Verror = Vsetpoint - currentVelocityInMPS;
                IntegralSUM += Verror * deltaTime;

                float KPterm = Verror * train_controller_ui->get_kp();
                float KIterm = train_controller_ui->get_ki() * IntegralSUM;

                float powerCommand = KPterm + KIterm;

                if (powerCommand > maxPower) {
                    powerCommand = maxPower;
                } else if (powerCommand < 0) {
                    powerCommand = 0;
                }

                train_controller_ui->set_commanded_power(powerCommand);
            }
        }
    });

    
    // POWER EQUATION
    // SERVICE BRAKE LEVELS
    // SPEED LIMITER


    train_controller_ui->on_request_update_current_velocity([&]{
        float temp = std::stof(std::string(train_controller_ui->get_temp_current_velocity()));

        train_controller_ui->set_current_velocity(temp);

        // Include calculations here
        // *** Begin Calculations ***
        float blockSpeedLimitMph = 50.0; // default speed limit
        float current_velocity_mph = train_controller_ui->get_current_velocity();
        float driver_speed_mph = train_controller_ui->get_driver_speed();

        // Enforce block speed limit
        if (current_velocity_mph > blockSpeedLimitMph) {
            current_velocity_mph = blockSpeedLimitMph;
            train_controller_ui->set_current_velocity(current_velocity_mph);
        }

        // Convert speeds to m/s
        float currentVelocityInMPS = current_velocity_mph * 0.44704;
        float driverSpeedInMPS = driver_speed_mph * 0.44704;
        float blockSpeedLimitMPS = blockSpeedLimitMph * 0.44704;

        // Enforce speed limit on driver speed
        float Vsetpoint = driverSpeedInMPS;
        if (Vsetpoint > blockSpeedLimitMPS) {
            Vsetpoint = blockSpeedLimitMPS;
        }

        // Time delta
        float deltaTime = 1;

        if(train_controller_ui->get_emergency_brake() == true || train_controller_ui->get_authority() == 475)
        {
            IntegralSUM = 0;
            train_controller_ui->set_commanded_power(0);
        }
        else
        {
            if (currentVelocityInMPS > Vsetpoint) {
                IntegralSUM = 0;
                train_controller_ui->set_commanded_power(0);

                float tempSpeedDiffMPS = currentVelocityInMPS - Vsetpoint;
                float tempSpeedDiffMPH = tempSpeedDiffMPS / 0.44704;

                // Implement service brake increments based on speed difference
                int brakeLevel = 0;
                if(tempSpeedDiffMPH > 0 && tempSpeedDiffMPH <= 4.35)
                    brakeLevel = 10;
                else if(tempSpeedDiffMPH > 4.35 && tempSpeedDiffMPH <= 8.7)
                    brakeLevel = 20;
                else if(tempSpeedDiffMPH > 8.7 && tempSpeedDiffMPH <= 13.05)
                    brakeLevel = 30;
                else if(tempSpeedDiffMPH > 13.05 && tempSpeedDiffMPH <= 17.4)
                    brakeLevel = 40;
                else if(tempSpeedDiffMPH > 17.4 && tempSpeedDiffMPH <= 21.75)
                    brakeLevel = 50;
                else if(tempSpeedDiffMPH > 21.75 && tempSpeedDiffMPH <= 26.1)
                    brakeLevel = 60;
                else if(tempSpeedDiffMPH > 26.1 && tempSpeedDiffMPH <= 30.45)
                    brakeLevel = 70;
                else if(tempSpeedDiffMPH > 30.45 && tempSpeedDiffMPH <= 34.8)
                    brakeLevel = 80;
                else if(tempSpeedDiffMPH > 34.8 && tempSpeedDiffMPH <= 39.15)
                    brakeLevel = 90;
                else if(tempSpeedDiffMPH > 39.15)
                    brakeLevel = 100;

                train_controller_ui->set_service_brake(brakeLevel);

            } else {
                if (train_controller_ui->get_service_brake() > 0) {
                    train_controller_ui->set_service_brake(0);
                }

                float Verror = Vsetpoint - currentVelocityInMPS;
                IntegralSUM += Verror * deltaTime;

                float KPterm = Verror * train_controller_ui->get_kp();
                float KIterm = train_controller_ui->get_ki() * IntegralSUM;

                float powerCommand = KPterm + KIterm;

                if (powerCommand > maxPower) {
                    powerCommand = maxPower;
                } else if (powerCommand < 0) {
                    powerCommand = 0;
                }

                train_controller_ui->set_commanded_power(powerCommand);
            }
        }
    });

    train_controller_ui->run();

    return 0;
}