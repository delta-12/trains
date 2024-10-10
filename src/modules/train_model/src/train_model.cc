#include "train_model.h"

int main()
{
    auto ui = TrainModel::create();

    ui->on_comm_speed_change([&]{
        float temp = std::stof(std::string(ui->get_comm_speed_in()));
            ui->set_comm_speed(temp);
    });
    ui->on_authority_change([&]{
        float temp = std::stof(std::string(ui->get_authority_in()));
            ui->set_authority(temp);
    });
    ui->on_pass_board_change([&]{
        int temp = std::stoi(std::string(ui->get_pass_board_in()));
            ui->set_passengers_boarding(temp);
    });
    ui->on_track_pol_change([&]{
        bool temp = (std::string(ui->get_track_pol_in()) == "true");
            ui->set_track_pol(temp);
    });
    ui->on_left_door_change([&]{
        bool temp = (std::string(ui->get_left_door_in()) == "true");
            ui->set_left_doors(temp);
    });
    ui->on_right_door_change([&]{
        bool temp = (std::string(ui->get_right_door_in()) == "true");
            ui->set_right_doors(temp);
    });
        ui->on_ext_light_change([&]{
        bool temp = (std::string(ui->get_ext_light_in()) == "true");
            ui->set_ext_lights(temp);
    });
        ui->on_int_light_change([&]{
        bool temp = (std::string(ui->get_int_light_in()) == "true");
            ui->set_int_lights(temp);
    });
        ui->on_comm_power_change([&]{
        float temp = std::stof(std::string(ui->get_comm_power_in()));
            ui->set_actual_power(temp);
    });
        ui->on_dist_trav_change([&]{
        int temp = std::stoi(std::string(ui->get_dist_trav_in()));
            ui->set_distance_traveled(temp);
    });
        ui->on_int_temp_change([&]{
        int temp = std::stoi(std::string(ui->get_int_temp_in()));
            ui->set_cabin_temp(temp);
    });
        ui->on_grade_change([&]{
        float temp = std::stof(std::string(ui->get_grade_in()));
            ui->set_grade(temp);
    });
        ui->on_service_brake_change([&]{
        float temp = std::stof(std::string(ui->get_service_brake_in()));
            ui->set_service_brake(temp);
    });
        ui->on_ebrake_change([&]{
        bool temp = (std::string(ui->get_ebrake_in()) == "true");
            ui->set_ebrake(temp);
    });
        ui->on_station_announce_change([&]{
            ui->set_station_banner(ui->get_station_announce_in());
    });


    ui->run();
    return 0;
}
