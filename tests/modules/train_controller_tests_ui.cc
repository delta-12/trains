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

    // operation mode

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

    // failures

    train_controller_ui->on_request_engine_fail([&] {
        if (train_controller_ui->get_engine_status() == 0)
            train_controller_ui->set_engine_status(1);
        else
            train_controller_ui->set_engine_status(0);
    });
    train_controller_ui->on_request_brake_fail([&] {
        if (train_controller_ui->get_brake_status() == 0)
            train_controller_ui->set_brake_status(1);
        else
            train_controller_ui->set_brake_status(0);
    });
    train_controller_ui->on_request_signal_fail([&] {
        if (train_controller_ui->get_signal_status() == 0)
            train_controller_ui->set_signal_status(1);
        else
            train_controller_ui->set_signal_status(0);
    });


    // Manual - UI input parameters

   // commanded speed input from test bench
    train_controller_ui->on_request_update_commanded_speed([&]{
        float temp = std::stof(std::string(train_controller_ui->get_temp_commanded_speed()));
            train_controller_ui->set_commanded_speed(temp);
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

    train_controller_ui->on_request_update_driver_speed([&] {
        float temp = std::stof(std::string(train_controller_ui->get_temp_driver_speed()));
            train_controller_ui->set_driver_speed(temp);
    });

    // current velocity input from test bench
    train_controller_ui->on_request_update_current_velocity([&]{
        float temp = std::stof(std::string(train_controller_ui->get_temp_current_velocity()));

            // *** CALCULATE COMMANDED POWER ***

            float blockSpeedLimit = 50.0; // default speed limit atm
            types::MetersPerSecond Vsetpoint = train_controller_ui->get_driver_speed() * 0.44704; // go to m/s

            if (train_controller_ui->get_current_velocity() > blockSpeedLimit) {
                train_controller_ui->set_current_velocity(50);
            } else {
                train_controller_ui->set_current_velocity(temp);
            }
            

            // convert to m/s from km/hr
            blockSpeedLimit *= 0.27778;

            if (Vsetpoint > blockSpeedLimit) {
                Vsetpoint = blockSpeedLimit;
            }

            // Temp time passed since last update
            float deltaTime = 1;

            if(train_controller_ui->get_emergency_brake() == 1 || train_controller_ui->get_authority() == 475)
            {
                IntegralSUM = 0;
                train_controller_ui->set_commanded_power(0);
            }
            else
  { 
    //Checking if Current Train Velocity is greater than Setpoint speed
    if (train_controller_ui->get_current_velocity()*0.4470272687 > Vsetpoint) {
        IntegralSUM = 0;
        train_controller_ui->set_commanded_power(0);

        float tempSpeedDiff = train_controller_ui->get_current_velocity() - train_controller_ui->get_driver_speed();

        if(tempSpeedDiff >0  && tempSpeedDiff <= 4.35)
        {
            train_controller_ui->set_service_brake(10);
        }
        else if (tempSpeedDiff > 4.35 && tempSpeedDiff <= 8.7) 
        {
            train_controller_ui->set_service_brake(20);
        }
        else if(tempSpeedDiff >8.7  && tempSpeedDiff <= 13.05)
        {
            train_controller_ui->set_service_brake(30);
        }
        else if (tempSpeedDiff > 13.05 && tempSpeedDiff <= 17.4) 
        {
            train_controller_ui->set_service_brake(40);
        }
        else if (tempSpeedDiff > 17.4 && tempSpeedDiff <= 21.75) 
        {
            train_controller_ui->set_service_brake(50);
        }
        else if (tempSpeedDiff > 21.75 && tempSpeedDiff <= 26.1) 
        {
            train_controller_ui->set_service_brake(60);
        }
        else if (tempSpeedDiff > 26.1 && tempSpeedDiff <= 30.45) 
        {
            train_controller_ui->set_service_brake(70);
        }
        else if (tempSpeedDiff > 30.45 && tempSpeedDiff <= 34.8) 
        {
            train_controller_ui->set_service_brake(80);
        }
        else if (tempSpeedDiff > 34.8 && tempSpeedDiff <= 39.15) 
        {
            train_controller_ui->set_service_brake(90);
        }
        else if(tempSpeedDiff > 39.15)
        {
            train_controller_ui->set_service_brake(100);
        }
    }
    //Checking if Service brake is on
    else if(train_controller_ui->get_service_brake() > 0)
    {
        IntegralSUM = 0;
        train_controller_ui->set_commanded_power(0);
    }

    //Normal power calculation
    else if(Vsetpoint > train_controller_ui->get_current_velocity()*0.4470272687) {
        float Verror = Vsetpoint - (train_controller_ui->get_current_velocity() * 0.4470272687);

    // Calculating Kp term
    float KPterm = Verror * train_controller_ui->get_kp();
    IntegralSUM += Verror * deltaTime;
    float KIterm = train_controller_ui->get_ki() * IntegralSUM;

    train_controller_ui->set_commanded_power(KPterm + KIterm);

    if (train_controller_ui->get_commanded_power() > maxPower) 
    {
        train_controller_ui->set_commanded_power(maxPower);
    }
    }
  }

    });

    train_controller_ui->on_request_update_service_brake([&] {
        // assign service brake variable to text input
    });
    
    // maybe emergency brake

    // Test Bench input parameters

    
    train_controller_ui->on_request_update_authority([&]{
        // assign authority variable to text input
    });

    train_controller_ui->run();

    return 0;
}