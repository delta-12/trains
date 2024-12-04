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
#include "block_builder.h"
#include "csv_parser.h"
#include "simulator.h"
#include "tick_source.h"
#include "track_model.h"
#include "train_model.h"
#include "train_controller.h"



int main(void)
{

    simulator::Simulator world;
    std::map<int,std::shared_ptr<train_controller::SoftwareTrainController>> train_controllers;
    auto                 launcher_ui           = ui::Launcher::create();
    auto                 ctc_ui                = ui::CtcUi::create();
    auto                 wayside_controller_ui = ui::WaysideControllerUi::create();
    auto                 track_model_ui        = ui::TrackModelUi::create();
    auto                 train_model_ui        = ui::TrainModelUi::create();
    auto                 train_controller_ui   = ui::TrainControllerUi::create();
    slint::Timer timer;

    

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

    types::Meters dist = 0;

    std::thread worker_thread([&world, &train_controllers, &dist, &train_controller_ui]
    {
        std::shared_ptr<TickSource> tick_source = std::make_shared<TickSource>();
        tick_source.get()->Start();
        tick_source.get()->SetMultiplier(3);
        slint::Timer timer;


        std::filesystem::path           base_path = std::filesystem::current_path();
        std::filesystem::path           path      = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_path.csv";
        std::filesystem::path           path2     = base_path / ".." / "tests" / "common" / "test_csv" / "green_line_track_layout.csv";
        CsvParser                       parser(path);
        CsvParser                       parser2(path2);
        BlockBuilder                    bb(parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
        BlockBuilder                    bb2(parser2.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
        
        
        std::shared_ptr<train_model::TrainModel> train = std::make_shared<train_model::SoftwareTrainModel>(tick_source);
        std::shared_ptr<train_controller::SoftwareTrainController> train_contr = std::make_shared<train_controller::SoftwareTrainController>(tick_source);
        std::shared_ptr<track_model::SoftwareTrackModel> track = std::make_shared<track_model::SoftwareTrackModel>();


        train.get()->SetTrainId(0);
        train_controllers[0] = train_contr;
        track->SetTrackLayout(types::TrackId::TRACKID_GREEN, bb.GetBlocks(), bb2.GetBlocks());
        world.AddTrackModel(track);
        world.AddTrainModel(track->GetTrackId(), train);
//        bool debugger = false;

        types::Blocks ATH = 10;
        
        while(1)
        {
            dist = train_controllers[0].get()->GetDistanceTravelled();
            
            if(dist == 0)
            {
                track.get()->SetAuthority(0,ATH);
                track.get()->SetCommandedSpeed(0,10);
            }

            types::Watts commanded_power = train_controllers[0].get()->GetCommandedPower();
            types::Meters distance_traveled_since_last_update = train_controllers[0].get()->GetDistanceTravelledSinceLastUpdate();
            double brake = train_controllers[0].get()->GetServiceBrake();


            track.get()->GetTrainModel(0).get()->SetCommandedPower(commanded_power);
            track.get()->GetTrainModel(0).get()->SetDistanceTraveled(distance_traveled_since_last_update);
            track.get()->GetTrainModel(0).get()->SetBrake(brake);
            
            if (dist > 100 && dist <= 200)
            {
                track.get()->SetAuthority(64,ATH-1);
                track.get()->SetCommandedSpeed(64,10);
            }
            else if(dist > 200 && dist <= 400)
            {
                track.get()->SetAuthority(65,ATH-2);
                track.get()->SetCommandedSpeed(65,10);
            }
            else if(dist > 400 && dist <= 600)
            {
                track.get()->SetAuthority(66,ATH-3);
                track.get()->SetCommandedSpeed(66,10);
            }
            else if(dist > 600 && dist <= 700)
            {
                track.get()->SetAuthority(67,ATH-4);
                track.get()->SetCommandedSpeed(67,10);
            }
            else if(dist > 700 && dist <= 800)
            {
                track.get()->SetAuthority(68,ATH-5);
                track.get()->SetCommandedSpeed(68,10);
            }
            else if(dist > 800 && dist <= 900)
            {
                track.get()->SetAuthority(69,ATH-6);
                track.get()->SetCommandedSpeed(69,10);
            }
            else if(dist > 900 && dist <= 1000)
            {
                track.get()->SetAuthority(70,ATH-7);
                track.get()->SetCommandedSpeed(70,10);
            }
            else if(dist > 1000 && dist <= 1100)
            {
                track.get()->SetAuthority(71,ATH-8);
                track.get()->SetCommandedSpeed(71,10);
            }
            else if(dist > 1100 && dist <= 1200)
            {
                track.get()->SetAuthority(72,ATH-9);
                track.get()->SetCommandedSpeed(72,10);
            }
            else if(dist > 1200 && dist <= 1300)
            {
                track.get()->SetAuthority(73,ATH-10);
                track.get()->SetCommandedSpeed(73,10);
            }


            world.Update();


            types::Polarity polarity = track.get()->GetTrainModel(0).get()->GetTrackPolarity();
            types::Blocks authority = track.get()->GetTrainModel(0).get()->GetAuthority();
            types::Meters commanded_speed = track.get()->GetTrainModel(0).get()->GetCommandedSpeed();
            types::MetersPerSecond current_speed = track.get()->GetTrainModel(0).get()->GetActualSpeed();

            train_controllers[0].get()->SetCurrentSpeed(current_speed);
            train_controllers[0].get()->SetPolartity(polarity);
            train_controllers[0].get()->SetAuthority(authority);
            train_controllers[0].get()->SetCommandedSpeed(commanded_speed);

            train_controllers[0].get()->Update();




            if(authority == 0 && current_speed == 0)
            {
                train_controllers[0].get()->Update();
                break;
            }
        }
    });


    timer.start(slint::TimerMode::Repeated, std::chrono::milliseconds(500), [&train_controller_ui, &train_controllers]() {
        train_controller_ui->set_current_velocity(train_controllers[0].get()->GetCurrentSpeed());
        train_controller_ui->set_commanded_speed(train_controllers[0].get()->GetCommandedSpeed());
        train_controller_ui->set_authority(train_controllers[0].get()->GetAuthority());
        train_controller_ui->set_actual_internal_temperature(train_controllers[0].get()->GetActualInternalTemperature());
        train_controller_ui->set_distance_traveled(train_controllers[0].get()->GetDistanceTravelled());
        train_controller_ui->set_distance_traveled_since_last_update(train_controllers[0].get()->GetDistanceTravelledSinceLastUpdate());
        train_controller_ui->set_commanded_power(train_controllers[0].get()->GetCommandedPower());
        train_controller_ui->set_service_brake(train_controllers[0].get()->GetServiceBrake() * 100);
        train_controller_ui->set_int_lights(train_controllers[0].get()->GetInteriorLights());
        train_controller_ui->set_ext_lights(train_controllers[0].get()->GetHeadLights());
        train_controller_ui->set_left_doors(train_controllers[0].get()->GetLeftDoors());
        train_controller_ui->set_right_doors(train_controllers[0].get()->GetRightDoors());
        train_controller_ui->set_station_name(slint::SharedString(train_controllers[0].get()->GetCurrentStationName()));
        train_controller_ui->set_emergency_brake(train_controllers[0].get()->GetEmergencyBrake());
        train_controller_ui->set_kp(train_controllers[0]->GetKP());
        train_controller_ui->set_ki(train_controllers[0]->GetKI());
    });


    
    // TRAIN CONTROLLER CALLBACKS START


    // automatic mode
    train_controller_ui->on_request_automatic_mode([&] {
        if (train_controllers[0].get()->GetOperationMode() == 1) {
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
        if (train_controllers[0].get()->GetOperationMode() == 0) {
            train_controllers[0].get()->SetOperationMode(1);
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
                train_controllers[0].get()->SetCurrentSpeed(temp_current_velocity);
            } catch (const std::exception&) {
                inputError = true;
            }
        }

        // commanded speed
        auto temp_commanded_speed_str = std::string(train_controller_ui->get_temp_commanded_speed());
        if (!temp_commanded_speed_str.empty()) {
            try {
                float temp_commanded_speed = std::stof(temp_commanded_speed_str);
                train_controllers[0].get()->SetCommandedSpeed(temp_commanded_speed);
            } catch (const std::exception&) {
                inputError = true;
            }
        }

        // authority
        auto temp_authority_str = std::string(train_controller_ui->get_temp_authority());
        if (!temp_authority_str.empty()) {
            try {
                float temp_authority = std::stof(temp_authority_str);
                train_controllers[0].get()->SetAuthority(temp_authority);
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
                train_controllers[0].get()->SetActualInternalTemperature(temp_actual_internal_temperature);
            } catch (const std::exception&) {
                inputError = true;
            }
        }

        if (inputError)
        {
            inputError = false;
        }

        // call update
         train_controllers[0].get()->Update();

        // assign all UI elements
        train_controller_ui->set_current_velocity(train_controllers[0].get()->GetCurrentSpeed());
        train_controller_ui->set_commanded_speed(train_controllers[0].get()->GetCommandedSpeed());
        train_controller_ui->set_authority(train_controllers[0].get()->GetAuthority());
        train_controller_ui->set_actual_internal_temperature(train_controllers[0].get()->GetActualInternalTemperature());
        train_controller_ui->set_distance_traveled(train_controllers[0].get()->GetDistanceTravelled());
        train_controller_ui->set_distance_traveled_since_last_update(train_controllers[0].get()->GetDistanceTravelledSinceLastUpdate());
        train_controller_ui->set_commanded_power(train_controllers[0].get()->GetCommandedPower());
        train_controller_ui->set_service_brake(train_controllers[0].get()->GetServiceBrake() * 100);
        train_controller_ui->set_int_lights(train_controllers[0].get()->GetInteriorLights());
        train_controller_ui->set_ext_lights(train_controllers[0].get()->GetHeadLights());
        train_controller_ui->set_left_doors(train_controllers[0].get()->GetLeftDoors());
        train_controller_ui->set_right_doors(train_controllers[0].get()->GetRightDoors());
        train_controller_ui->set_station_name(slint::SharedString(train_controllers[0].get()->GetCurrentStationName()));
        train_controller_ui->set_emergency_brake(train_controllers[0].get()->GetEmergencyBrake());
        train_controller_ui->set_kp(train_controllers[0]->GetKP());
        train_controller_ui->set_ki(train_controllers[0]->GetKI());
    });

    // Update - End

    // Failures - Start

    // engine failure
    train_controller_ui->on_request_engine_fail([&] {
        if (train_controllers[0].get()->GetEngineFailure() == 0) {
            train_controllers[0].get()->SetEngineFailure(1);
            train_controllers[0].get()->SetEmergencyBrake(1);
            train_controllers[0].get()->SetCommandedPower(0);
            train_controllers[0].get()->SetServiceBrake(0);
            train_controller_ui->set_engine_status(train_controllers[0].get()->GetEngineFailure());
            train_controller_ui->set_emergency_brake(train_controllers[0].get()->GetEmergencyBrake());
            train_controller_ui->set_commanded_power(train_controllers[0].get()->GetCommandedPower());
            train_controller_ui->set_service_brake(train_controllers[0].get()->GetServiceBrake() * 100);
        } else {
            train_controllers[0].get()->SetEngineFailure(0);
            train_controller_ui->set_engine_status(train_controllers[0].get()->GetEngineFailure());
            train_controllers[0].get()->Update();
            train_controller_ui->set_emergency_brake(train_controllers[0].get()->GetEmergencyBrake());
            train_controller_ui->set_commanded_power(train_controllers[0].get()->GetCommandedPower());
        }
    });

    // brake failure
    train_controller_ui->on_request_brake_fail([&] {
        if (train_controllers[0].get()->GetBrakeFailure() == 0) {
            train_controllers[0].get()->SetBrakeFailure(1);
            train_controllers[0].get()->SetEmergencyBrake(1);
            train_controllers[0].get()->SetCommandedPower(0);
            train_controllers[0].get()->SetServiceBrake(0);
            train_controller_ui->set_brake_status(train_controllers[0].get()->GetBrakeFailure());
            train_controller_ui->set_emergency_brake(train_controllers[0].get()->GetEmergencyBrake());
            train_controller_ui->set_commanded_power(train_controllers[0].get()->GetCommandedPower());
            train_controller_ui->set_service_brake(train_controllers[0].get()->GetServiceBrake() * 100);
        } else {
            train_controllers[0].get()->SetBrakeFailure(0);
            train_controller_ui->set_brake_status(train_controllers[0].get()->GetBrakeFailure());
        }
    });

    // signal pickup failure
    train_controller_ui->on_request_signal_fail([&] {
        if (train_controllers[0].get()->GetSignalPickupFailure() == 0) {
            train_controllers[0].get()->SetSignalPickupFailure(1);
            train_controllers[0].get()->SetEmergencyBrake(1);
            train_controllers[0].get()->SetCommandedPower(0);
            train_controllers[0].get()->SetServiceBrake(0);
            train_controller_ui->set_signal_status(train_controllers[0].get()->GetSignalPickupFailure());
            train_controller_ui->set_emergency_brake(train_controllers[0].get()->GetEmergencyBrake());
            train_controller_ui->set_commanded_power(train_controllers[0].get()->GetCommandedPower());
            train_controller_ui->set_service_brake(train_controllers[0].get()->GetServiceBrake() * 100);
        } else {
            train_controllers[0].get()->SetSignalPickupFailure(0);
            train_controller_ui->set_signal_status(train_controllers[0].get()->GetSignalPickupFailure());
        }
    });

    // Failures - End

    // Manual - UI input parameters - Start

    // service brake input
    train_controller_ui->on_request_update_service_brake([&] {
        if (train_controllers[0].get()->GetOperationMode() == 1)
        {
            float temp = std::stof(std::string(train_controller_ui->get_temp_service_brake()));
            train_controllers[0].get()->SetServiceBrake(temp);
            //train_controllers[0].get()->SetCommandedPower(0);
            train_controllers[0].get()->Update();
            train_controller_ui->set_service_brake(train_controllers[0].get()->GetServiceBrake() * 100);
            train_controller_ui->set_commanded_power(train_controllers[0].get()->GetCommandedPower());
            train_controller_ui->set_distance_traveled(train_controllers[0].get()->GetDistanceTravelled());
        }
    });

    // commanded internal temperature
    train_controller_ui->on_request_update_commanded_internal_temperature([&] {
        if (train_controllers[0].get()->GetOperationMode() == 1) {
            float temp = std::stof(std::string(train_controller_ui->get_temp_commanded_internal_temperature()));
            if (temp < 65) {
                temp = 65;
            } else if (temp > 75) {
                temp = 75;
            }
            train_controllers[0].get()->SetCommandedInternalTemperature(temp);
            train_controller_ui->set_commanded_internal_temperature(train_controllers[0].get()->GetCommandedInternalTemperature());
        }
    });

    // driver speed
    train_controller_ui->on_request_update_driver_speed([&] {
        if (train_controllers[0].get()->GetOperationMode() == 1) 
        {
            float temp = std::stof(std::string(train_controller_ui->get_temp_driver_speed()));
            train_controllers[0].get()->SetDriverSpeed(temp);
            train_controllers[0].get()->Update();
            train_controller_ui->set_driver_speed(train_controllers[0].get()->GetDriverSpeed());
            train_controller_ui->set_commanded_power(train_controllers[0].get()->GetCommandedPower());
            train_controller_ui->set_service_brake(train_controllers[0].get()->GetServiceBrake() * 100);
            train_controller_ui->set_distance_traveled(train_controllers[0].get()->GetDistanceTravelled());
            train_controller_ui->set_distance_traveled_since_last_update(train_controllers[0].get()->GetDistanceTravelledSinceLastUpdate());
        }
    });

    // Interior Lights
    train_controller_ui->on_request_toggle_int_lights([&] {
        if (train_controllers[0].get()->GetOperationMode() == 1 && !train_controllers[0].get()->IsAtStation()) {
            bool current_state = train_controllers[0].get()->GetInteriorLights();
            train_controllers[0].get()->SetInteriorLights(!current_state);
            train_controller_ui->set_int_lights(train_controllers[0].get()->GetInteriorLights());
        }
    });

    // Exterior Lights
    train_controller_ui->on_request_toggle_ext_lights([&] {
        if (train_controllers[0].get()->GetOperationMode() == 1 && !train_controllers[0].get()->IsUnderground()) {
            bool current_state = train_controllers[0].get()->GetHeadLights();
            train_controllers[0].get()->SetHeadLights(!current_state);
            train_controller_ui->set_ext_lights(train_controllers[0].get()->GetHeadLights());
        } else if (train_controllers[0].get()->IsUnderground()) {
            // Do not allow turning off exterior lights when underground
            train_controllers[0].get()->SetHeadLights(true);
            train_controller_ui->set_ext_lights(true);
        }
    });

    // Left Doors
    train_controller_ui->on_request_toggle_left_doors([&] {
        if (train_controllers[0].get()->GetOperationMode() == 1 && train_controllers[0].get()->CanOpenDoors()) {
            // Doors will open automatically at stations
            // Allow manual closing
            bool current_state = train_controllers[0].get()->GetLeftDoors();
            if (current_state) {
                train_controllers[0].get()->SetLeftDoors(false);
                train_controller_ui->set_left_doors(false);
            }
        }
    });

    // Right Doors
    train_controller_ui->on_request_toggle_right_doors([&] {
        if (train_controllers[0].get()->GetOperationMode() == 1 && train_controllers[0].get()->CanOpenDoors()) {
            // Doors will open automatically at stations
            // Allow manual closing
            bool current_state = train_controllers[0].get()->GetRightDoors();
            if (current_state) {
                train_controllers[0].get()->SetRightDoors(false);
                train_controller_ui->set_right_doors(false);
            }
        }
    });

    // Emergency brake
    train_controller_ui->on_request_toggle_emergency_brake([&] {
        bool current_state = train_controllers[0]->GetEmergencyBrake();
        train_controllers[0]->SetEmergencyBrake(!current_state);
        train_controller_ui->set_emergency_brake(!current_state);
    });

    // Update Kp and Ki values
    train_controller_ui->on_request_update_kp_ki([&] {
        if (train_controllers[0]->GetDistanceTravelled() == 0)
        {
            // Get the temporary kp and ki values from the UI
            auto temp_kp_str = std::string(train_controller_ui->get_temp_kp());
            auto temp_ki_str = std::string(train_controller_ui->get_temp_ki());

            bool inputError = false;

            // Parse and set Kp
            if (!temp_kp_str.empty()) {
                try {
                    uint16_t temp_kp = static_cast<uint16_t>(std::stoul(temp_kp_str));
                    train_controllers[0]->SetKP(temp_kp);
                    // Update UI
                    train_controller_ui->set_kp(temp_kp);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            // Parse and set Ki
            if (!temp_ki_str.empty()) {
                try {
                    uint16_t temp_ki = static_cast<uint16_t>(std::stoul(temp_ki_str));
                    train_controllers[0]->SetKI(temp_ki);
                    // Update UI
                    train_controller_ui->set_ki(temp_ki);
                } catch (const std::exception&) {
                    inputError = true;
                }
            }

            if (inputError) {
                // Handle input error (e.g., display a message to the user)
                std::cout << "Invalid Kp or Ki value entered." << std::endl;
            }
        } else {
            // Cannot update kp and ki after the train has been dispatched
            std::cout << "Cannot update Kp and Ki after the train has been dispatched." << std::endl;
        }
    });

    // Reset Kp and Ki to default values
    train_controller_ui->on_request_reset_kp_ki([&] {
        if (train_controllers[0]->GetDistanceTravelled() == 0)
        {
            // Reset to default values
            train_controllers[0]->SetKP(5000);
            train_controllers[0]->SetKI(100);

            // Update UI
            train_controller_ui->set_kp(5000);
            train_controller_ui->set_ki(100);

            // Clear temporary inputs
            train_controller_ui->set_temp_kp("");
            train_controller_ui->set_temp_ki("");
        } else {
            // Cannot reset kp and ki after the train has been dispatched
            std::cout << "Cannot reset Kp and Ki after the train has been dispatched." << std::endl;
        }
    });


    // TRAIN CONTROLLER CALLBACKS END


    //std::cout << x;
    launcher_ui->run();
    worker_thread.join();
    //ui_thread.join();


    timer.stop();
    return 0;
}