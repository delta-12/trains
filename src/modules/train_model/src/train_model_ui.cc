// #include <thread>

// #include <slint.h>

// #include "launcher.h"

// using namespace std;

// int main()
// {

//     auto ui = TrainModelUi::create();

// train_model_ui->on_comm_speed_change([&]{
//     float temp = std::stof(std::string(train_model_ui->get_comm_speed_in()));
//     train_model_ui->set_comm_speed(temp);
// });
// train_model_ui->on_authority_change([&]{
//     float temp = std::stof(std::string(train_model_ui->get_authority_in()));
//     train_model_ui->set_authority(temp);
// });
// train_model_ui->on_pass_board_change([&]{
//     int temp = std::stoi(std::string(train_model_ui->get_pass_board_in()));
//     train_model_ui->set_passengers_boarding(temp);
// });
// train_model_ui->on_track_pol_change([&]{
//     bool temp = (std::string(train_model_ui->get_track_pol_in()) == "true");
//     train_model_ui->set_track_pol(temp);
// });
// train_model_ui->on_left_door_change([&]{
//     bool temp = (std::string(train_model_ui->get_left_door_in()) == "true");
//     train_model_ui->set_left_doors(temp);
// });
// train_model_ui->on_right_door_change([&]{
//     bool temp = (std::string(train_model_ui->get_right_door_in()) == "true");
//     train_model_ui->set_right_doors(temp);
// });
// train_model_ui->on_ext_light_change([&]{
//     bool temp = (std::string(train_model_ui->get_ext_light_in()) == "true");
//     train_model_ui->set_ext_lights(temp);
// });
// train_model_ui->on_int_light_change([&]{
//     bool temp = (std::string(train_model_ui->get_int_light_in()) == "true");
//     train_model_ui->set_int_lights(temp);
// });
// train_model_ui->on_comm_power_change([&]{
//     float temp = std::stof(std::string(train_model_ui->get_comm_power_in()));
//     train_model_ui->set_actual_power(temp);
//     train_model_ui->set_acceleration(temp / 80800);
//     train_model_ui->set_act_speed(train_model_ui->get_act_speed() + temp / 20000);
// });
// train_model_ui->on_dist_trav_change([&]{
//     int temp = std::stoi(std::string(train_model_ui->get_dist_trav_in()));
//     train_model_ui->set_distance_traveled(temp);
// });
// train_model_ui->on_int_temp_change([&]{
//     int temp = std::stoi(std::string(train_model_ui->get_int_temp_in()));
//     train_model_ui->set_cabin_temp(temp);
// });
// train_model_ui->on_grade_change([&]{
//     float temp = std::stof(std::string(train_model_ui->get_grade_in()));
//     train_model_ui->set_grade(temp);
// });
// train_model_ui->on_service_brake_change([&]{
//     float temp = std::stof(std::string(train_model_ui->get_service_brake_in()));
//     train_model_ui->set_service_brake(temp);
// });
// train_model_ui->on_ebrake_change([&]{
//     bool temp = (std::string(train_model_ui->get_ebrake_in()) == "true");
//     train_model_ui->set_ebrake(temp);
// });
// train_model_ui->on_station_announce_change([&]{
//     train_model_ui->set_station_banner(train_model_ui->get_station_announce_in());
// });


//     train_model_ui->run();
//     return 0;
// }
