#include <chrono>
#include <iostream>
#include <thread>
#include <filesystem>
#include <string>

#include <slint.h>

#include "launcher.h"
#include "simulator.h"
#include "types.h"
#include "csv_parser.h"
#include "block_builder.h"
#include "train_model.h"
#include "track_model.h"
#include "random_number_generator.h"
#include "channel.h"

namespace train_model
{

// Definition of the TrainModelImpl class
class TrainModelImpl : public TrainModel
{
    public:
        void SetTrainId(const types::TrainId train);
        types::TrainId GetTrainId(void) const;
        void SetEmergencyBrake(const bool emergency_brake);
        void SetEngineFailure(const bool engine_failure);
        void SetBrakeFailure(const bool brake_failure);
        void SetSignalPickupFailure(const bool signal_pickup_failure);
        bool GetBrakeFailure(void) const;
        std::size_t GetBeaconData(types::BeaconData &data) const;
        types::Blocks GetAuthority(void) const;
        types::MetersPerSecond GetCommandedSpeed(void) const;
        types::MetersPerSecond GetActualSpeed(void) const;
        bool GetEngineFailure(void) const;
        bool GetSignalPickupFailure(void) const;
        types::Watts GetActualPower(void) const;
        types::DegreesFahrenheit GetActualInternalTemperature(void) const;
        types::Polarity GetTrackPolarity(void) const;
        void SetStationAnnouncement(const std::string &announcement);
        void SetHeadlights(const bool on);
        void SetInternalLights(const bool on);
        void SetLeftDoorsState(const bool open);
        void SetRightDoorsState(const bool open);
        void SetCommandedPower(const types::Watts power);
        void SetCommandedInternalTemperature(const types::DegreesFahrenheit degrees);
        void SetBlockId(const types::BlockId block);
        uint16_t GetPassengersDeboarding(void);
        types::Meters GetDistanceTraveled(void) const;
        void SetCommandedSpeed(const types::MetersPerSecond speed);
        void SetAuthority(const types::Blocks blocks);
        void SetPassengersBoarding(const uint16_t passengers);
        void SetTrackPolarity(const types::Polarity polarity);
        void SetBeaconData(const types::BeaconData &data, std::size_t &size);
        void SetDistanceTraveled(const types::Meters distance);
        void SetGrade(const double grade);
        void SetBrake(const double brake);
        void SetPassengersDeboarding(uint16_t passengers);

    private:
        types::TrainId train_id_;
        bool emergency_brake_;
        bool engine_failure_;
        bool brake_failure_;
        bool signal_pickup_failure_;
        types::Meters commanded_speed_;
        types::MetersPerSecond actual_speed_;
        types::Blocks authority_;
        uint16_t current_passengers_ = 0;
        bool left_doors_open_;
        bool right_doors_open_;
        types::DegreesFahrenheit internal_temperature_;
        types::Watts commanded_power_;
        types::Watts actual_power_;
        types::Polarity track_polarity_;
        std::string announcement_;
        bool headlights_on_;
        bool internal_lights_on_;
        types::BlockId block_id_;
        double grade_;
        double brake_;
        types::Meters distance_traveled_ = 100;
        uint16_t passengers_deboarding_=0;
};

void TrainModelImpl::SetGrade(const double grade)
{
    grade_ = grade;
}

void TrainModelImpl::SetBrake(const double brake)
{
    brake_ = brake;
}

void TrainModelImpl::SetTrainId(const types::TrainId train)
{
    train_id_ = train;
}

types::TrainId TrainModelImpl::GetTrainId(void) const
{
    return 1;
}

void TrainModelImpl::SetEmergencyBrake(const bool emergency_brake)
{
    emergency_brake_ = emergency_brake;
}

void TrainModelImpl::SetEngineFailure(const bool engine_failure)
{
    engine_failure_ = engine_failure;
}

void TrainModelImpl::SetBrakeFailure(const bool brake_failure)
{
    brake_failure_ = brake_failure;
}

void TrainModelImpl::SetSignalPickupFailure(const bool signal_pickup_failure)
{
    signal_pickup_failure_ = signal_pickup_failure;
}

bool TrainModelImpl::GetBrakeFailure(void) const
{
    return brake_failure_;
}

std::size_t TrainModelImpl::GetBeaconData(types::BeaconData &data) const
{
    // Stub for getting beacon data, return size of data
    return sizeof(data);
}

types::Blocks TrainModelImpl::GetAuthority(void) const
{
    return authority_;
}

types::MetersPerSecond TrainModelImpl::GetCommandedSpeed(void) const
{
    return commanded_speed_;
}

types::MetersPerSecond TrainModelImpl::GetActualSpeed(void) const
{
    return actual_speed_;
}

bool TrainModelImpl::GetEngineFailure(void) const
{
    return engine_failure_;
}

bool TrainModelImpl::GetSignalPickupFailure(void) const
{
    return signal_pickup_failure_;
}

types::Watts TrainModelImpl::GetActualPower(void) const
{
    return actual_power_;
}

types::DegreesFahrenheit TrainModelImpl::GetActualInternalTemperature(void) const
{
    return internal_temperature_;
}

types::Polarity TrainModelImpl::GetTrackPolarity(void) const
{
    return track_polarity_;
}

void TrainModelImpl::SetDistanceTraveled(const types::Meters distance)
{
    distance_traveled_=distance;
}

void TrainModelImpl::SetStationAnnouncement(const std::string &announcement)
{
    // Handle station announcement
    announcement_ = announcement;
}

void TrainModelImpl::SetHeadlights(const bool on)
{
    headlights_on_ = on;
}

void TrainModelImpl::SetInternalLights(const bool on)
{
    internal_lights_on_ = on;
}

void TrainModelImpl::SetLeftDoorsState(const bool open)
{
    left_doors_open_ = open;
}

void TrainModelImpl::SetRightDoorsState(const bool open)
{
    right_doors_open_ = open;
}

void TrainModelImpl::SetCommandedPower(const types::Watts power)
{
    commanded_power_ = power;
}

void TrainModelImpl::SetCommandedInternalTemperature(const types::DegreesFahrenheit degrees)
{
    internal_temperature_ = degrees;
}

void TrainModelImpl::SetBlockId(const types::BlockId block)
{
    block_id_ = block;
}

uint16_t TrainModelImpl::GetPassengersDeboarding(void)
{
    return passengers_deboarding_;
}

void TrainModelImpl::SetPassengersDeboarding(uint16_t passengers)
{
    passengers_deboarding_=passengers;
}

types::Meters TrainModelImpl::GetDistanceTraveled(void) const
{
    return distance_traveled_;
}

void TrainModelImpl::SetCommandedSpeed(const types::MetersPerSecond speed)
{
    commanded_speed_ = speed;
}

void TrainModelImpl::SetAuthority(const types::Blocks blocks)
{
    authority_ = blocks;
}

void TrainModelImpl::SetPassengersBoarding(const uint16_t passengers)
{
    current_passengers_ += passengers;
}

void TrainModelImpl::SetTrackPolarity(const types::Polarity polarity)
{
    track_polarity_ = polarity;
}

void TrainModelImpl::SetBeaconData(const types::BeaconData &data, std::size_t &size)
{
    // Stub for setting beacon data
    size = sizeof(data);
}

} // namespace train_model


std::string ExtractFileName(const std::string& full_path) {
    // Find the last occurrence of backslash
    size_t pos = full_path.find_last_of("\\/");
    if (pos != std::string::npos) {
        return full_path.substr(pos + 1); // Return everything after the last backslash
    } else {
        return full_path; // If no backslash is found, return the full string (it may already be a file name)
    }
}

int main(void)
{
    simulator::Simulator world;
    auto                 launcher_ui           = ui::Launcher::create();
    auto                 ctc_ui                = ui::CtcUi::create();
    auto                 wayside_controller_ui = ui::WaysideControllerUi::create();
    auto                 track_model_ui        = ui::TrackModelUi::create();
    auto                 train_model_ui        = ui::TrainModelUi::create();
    auto                 train_controller_ui   = ui::TrainControllerUi::create();

    slint::ComponentWeakHandle<ui::TrackModelUi> weak_ui_handle(track_model_ui);

    track_model::SoftwareTrackModel track;
    train_model::TrainModelImpl     train;

    std::shared_ptr<train_model::TrainModel> ptr = std::make_shared<train_model::TrainModelImpl>(train);
    track.AddTrainModel(ptr);
    std::vector<std::shared_ptr<train_model::TrainModel>> train_ptrs;
    train_ptrs.push_back(ptr);

    std::string input_file_path="";
    std::string input_file_path2="";

    std::string color = "";

    std::filesystem::path exe_path = std::filesystem::current_path();
        
    track_model_ui->on_choose_file([&]() {
            // For other platforms
            std::string path;
            std::cout << "Please enter the path to the CSV file: ";

            std::getline(std::cin, input_file_path);

            if (input_file_path.find("green") != std::string::npos)
            {
                color = "green";
            }
            else if (input_file_path.find("red") != std::string::npos)
            {
                color = "red";
            }

            // Check if the file exists
            if (std::filesystem::exists(input_file_path)) {
                std::cout << "Selected file: " << input_file_path << std::endl;
                track_model_ui->set_selected_fileName(ExtractFileName(input_file_path).c_str());
            } else {
                std::cout << "File does not exist." << std::endl;
                track_model_ui->set_selected_fileName("No file selected");
            }

    track_model_ui->on_parse_file([&]() {
            std::cout << "Parsing File..." << std::endl;

            std::filesystem::path inorder_path(input_file_path);
            CsvParser inorder_parser(inorder_path);

            std::cout << "Track size: " << inorder_parser.GetSize() << std::endl;

            //std::string path2;
            //std::cout << "Please enter the path to the CSV file: ";

            //std::getline(std::cin, input_file_path2);

            //std::filesystem::path trackpath_path(input_file_path2);
            //std::filesystem::path hard_path_green("/Users/ibrah/Documents/trainscode/trains/tests/common/test_csv/green_line_path.csv");

            if (color == "green")
            {
                std::filesystem::path hard_path_green = exe_path / "tests" / "common" / "test_csv" / "green_line_path.csv";
                CsvParser trackpath_parser(hard_path_green);
                std::filesystem::path           base_path = std::filesystem::current_path();

                //std::filesystem::path           trackpath_path = base_path / ".." / "green_line_path.csv";
                //std::cout << trackpath_path;
                //CsvParser                       trackpath_parser(trackpath_path);
                BlockBuilder                    inorder_bb(inorder_parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
                BlockBuilder                    trackpath_bb(trackpath_parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
                
                track_model_ui->set_track_id("green");

                track.SetTrackLayout(types::TrackId::TRACKID_GREEN, trackpath_bb.GetBlocks(), inorder_bb.GetBlocks());
            }
            else if (color == "red")
            {
                std::filesystem::path hard_path_red = exe_path / "tests" / "common" / "test_csv" / "red_line_track_layout.csv";
                CsvParser trackpath_parser(hard_path_red);
                std::filesystem::path           base_path = std::filesystem::current_path();

                //std::filesystem::path           trackpath_path = base_path / ".." / "green_line_path.csv";
                //std::cout << trackpath_path;
                //CsvParser                       trackpath_parser(trackpath_path);
                BlockBuilder                    inorder_bb(inorder_parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
                BlockBuilder                    trackpath_bb(trackpath_parser.GetRecords(), RecordType::RECORDTYPE_TRACK_LAYOUT);
                
                track_model_ui->set_track_id("red");

                track.SetTrackLayout(types::TrackId::TRACKID_RED, trackpath_bb.GetBlocks(), inorder_bb.GetBlocks());
                }
                
            int thenumblocks=inorder_parser.GetSize();
            track_model_ui->set_num_blocks(std::to_string(thenumblocks).c_str());

            std::string line_color;

            if (track.GetTrackId() == types::TrackId::TRACKID_GREEN)
            {
                track_model_ui->set_line_type("GREEN");
            }
            else if (track.GetTrackId() == types::TrackId::TRACKID_RED)
            {
                track_model_ui->set_line_type("RED");
            }
        });

    });

    track_model_ui->on_request_update_tcfail_block([&]() {
        std::string string_block_num = std::string(track_model_ui->get_block_number());
        int int_block_num = std::stoi(string_block_num);
            bool tcfail_bool = track_model_ui->get_tcfail_bool();
            track.SetTrackCircuitFailure(int_block_num, tcfail_bool);
    });

    track_model_ui->on_request_update_pfail_block([&]() {
        std::string string_block_num = std::string(track_model_ui->get_block_number());
        int int_block_num = std::stoi(string_block_num);
            bool pfail_bool = track_model_ui->get_pfail_bool();
            track.SetPowerFailure(int_block_num, pfail_bool);
    });

    track_model_ui->on_request_update_brail_block([&]() {
        std::string string_block_num = std::string(track_model_ui->get_block_number());
        int int_block_num = std::stoi(string_block_num);
            bool brail_bool = track_model_ui->get_brail_bool();
            track.SetBrokenRail(int_block_num, brail_bool);
    });

    std::thread worker_thread([&]
    {
        slint::invoke_from_event_loop([&]() {
        // track_model_ui->on_request_update_train_block([&](){
        // if (auto ui = weak_ui_handle.lock()) {
        //     // Main backend loop here\
        //     /*track.Update();
        //     auto occupiedtrainblocks = track.GetOccupiedTrainBlocks();
        //     std::cout << "size is" << occupiedtrainblocks[0].size() << std::endl;
        //     float trainblock = occupiedtrainblocks[0][0];*/
        //     track_model_ui->set_train_block(64);
        //     }
        // });
        track_model_ui->on_request_update_distance_traveled([&](){
            if (auto ui = weak_ui_handle.lock()) {
            std::string d_traveled_string = std::string(track_model_ui->get_d_traveled());
            int d_traveled_int = std::stoi(d_traveled_string);
            std::string train_id_string = std::string(track_model_ui->get_train_id());
            int train_id = std::stoi(train_id_string);

            for (int i=0;i<train_ptrs.size();i++)
            {
                train_ptrs[i]->SetDistanceTraveled(0);
            }

            train_ptrs[train_id-1]->SetDistanceTraveled(d_traveled_int);
            track.Update();
            std::vector<std::shared_ptr<train_model::TrainModel>> trains;
            track.GetTrainModels(trains);
            std::string otb_string="";
            if (trains.empty())
            {
                track_model_ui->set_blocks_occupied_train("YARD");
                otb_string="";
            }
            else 
            {
                auto otb = track.GetOccupiedTrainBlocks();
                for (int j=0;j<otb.size();j++)
                {
                    for (int i=0;i<otb[j].size();i++)
                    {
                        otb_string+=std::to_string(otb[j][i]);
                        otb_string+=" ";
                    }
                }
                //track_model_ui->set_blocks_occupied(otb_string.c_str());
                track_model_ui->set_blocks_occupied_train(otb_string.c_str());

                std::string train_block_string;

                for (int k=0;k<trains.size();k++)
                {
                    train_block_string+=std::to_string(otb[k][0]);
                    train_block_string+=" ";
                }


                track_model_ui->set_train_block(train_block_string.c_str());
            }
                auto failed = track.GetFailedBlocks();
                for (int i=0;i<failed.size();i++)
                {
                    otb_string+=std::to_string(failed[i]);
                    otb_string+=" ";
                }
                track_model_ui->set_blocks_occupied(otb_string.c_str());
                // if (!trains.empty())
                // {
                //     track.GetTrainModels(trains);
                //     types::Blocks train_authority = trains[0]->GetAuthority();
                //     track_model_ui->set_train_authority(std::to_string(train_authority).c_str());
                // }
            }
        });

        track_model_ui->on_request_update_authority([&]() {
            if (auto ui = weak_ui_handle.lock()) {
            std::string string_authority_block = std::string(track_model_ui->get_authority_block());
            int int_authority_block = std::stoi(string_authority_block);
            std::string string_authority = std::string(track_model_ui->get_authority());
            int int_authority = std::stoi(string_authority);
            std::vector<std::shared_ptr<train_model::TrainModel>> trains;
            track.SetAuthority(int_authority_block, int_authority);
            track.GetTrainModels(trains);
            if (!trains.empty())
                {
                    types::Blocks train_authority = trains[0]->GetAuthority();
                    track_model_ui->set_train_authority(std::to_string(train_authority).c_str());
                }
            }
        });

        track_model_ui->on_request_add_train([&]() {
            if (auto ui = weak_ui_handle.lock()) {
                train_model::TrainModelImpl     train_temp;
                std::shared_ptr<train_model::TrainModel> ptr_temp = std::make_shared<train_model::TrainModelImpl>(train_temp);
                track.AddTrainModel(ptr_temp);
                train_ptrs.push_back(ptr_temp);
            }
        });

        track_model_ui->on_request_update_commanded_speed([&]() {
            if (auto ui = weak_ui_handle.lock()) {
            std::string string_commanded_speed_block = std::string(track_model_ui->get_commanded_speed_block());
            int int_commanded_speed_block = std::stoi(string_commanded_speed_block);
            std::string string_commanded_speed= std::string(track_model_ui->get_commanded_speed());
            int int_commanded_speed = std::stoi(string_commanded_speed);
            std::vector<std::shared_ptr<train_model::TrainModel>> trains;
            track.SetCommandedSpeed(int_commanded_speed_block, int_commanded_speed);
            track.GetTrainModels(trains);
            if (!trains.empty())
                {
                    types::Blocks train_commanded_speed = trains[0]->GetCommandedSpeed();
                    track_model_ui->set_train_commanded_speed(std::to_string(train_commanded_speed).c_str());
                }
            }
        });

        track_model_ui->on_request_update_passengers_deboarding([&]() {
            if (auto ui = weak_ui_handle.lock()) {
            std::string string_pass_deb = std::string(track_model_ui->get_passengers_deboarding());
            int int_pass_deb = std::stoi(string_pass_deb);
            // subtract from total passengers
            uint16_t vacancy = int_pass_deb;
            RandomNumberGenerator rng;
            uint16_t randomNumber = rng.generate(vacancy);
            std::string randomnumberstring = std::to_string(randomNumber);
            track_model_ui->set_passengers_boarding(randomnumberstring.c_str());
            }
        });

        track_model_ui->on_request_update_crossing_block([&]() {
            if (auto ui = weak_ui_handle.lock()) {
            std::string string_crossing_block = std::string(track_model_ui->get_crossing_block_tb());
            bool crossing_block_bool = track_model_ui->get_crossing_bool();
            int int_crossing_block = std::stoi(string_crossing_block);
            track.SetCrossingState(int_crossing_block, crossing_block_bool);
            }
        });

        track_model_ui->on_request_update_switch_block([&]() {
            if (auto ui = weak_ui_handle.lock()) {
            std::string string_switch_block = std::string(track_model_ui->get_switch_block_tb());
            bool switch_block_bool = track_model_ui->get_switch_bool();
            int int_switch_block = std::stoi(string_switch_block);
            track.SetSwitchState(int_switch_block, switch_block_bool);
            }
        });

        track_model_ui->on_request_update_green_block([&]() {
            if (auto ui = weak_ui_handle.lock()) {
            std::string string_green_block = std::string(track_model_ui->get_green_block());
            bool green_bool = track_model_ui->get_green_bool();
            int int_green_block = std::stoi(string_green_block);
            track.SetGreenTrafficLight(int_green_block, green_bool);
            }
        });

        track_model_ui->on_request_update_yellow_block([&]() {
            if (auto ui = weak_ui_handle.lock()) {
            std::string string_yellow_block = std::string(track_model_ui->get_yellow_block());
            bool yellow_bool = track_model_ui->get_yellow_bool();
            int int_yellow_block = std::stoi(string_yellow_block);
            track.SetYellowTrafficLight(int_yellow_block, yellow_bool);
            }
        });

        track_model_ui->on_request_update_red_block([&]() {
            if (auto ui = weak_ui_handle.lock()) {
            std::string string_red_block = std::string(track_model_ui->get_red_block());
            bool red_bool = track_model_ui->get_red_bool();
            int int_red_block = std::stoi(string_red_block);
            track.SetRedTrafficLight(int_red_block, red_bool);
            }
        });

        track_model_ui->on_request_update_tc_fail_block_tb([&]() {
            if (auto ui = weak_ui_handle.lock()) {
            std::string string_block_num = std::string(track_model_ui->get_tcfail_block());
            int int_block_num = std::stoi(string_block_num);
                bool tcfail_bool = track_model_ui->get_tcfail_bool_tb();
                track.SetTrackCircuitFailure(int_block_num, tcfail_bool);
                auto otb = track.GetOccupiedTrainBlocks();
                std::vector<std::shared_ptr<train_model::TrainModel>> trains;
                track.GetTrainModels(trains);
                std::string otb_string="";

                auto failed = track.GetFailedBlocks();
                for (int i=0;i<failed.size();i++)
                {
                    otb_string+=std::to_string(failed[i]);
                    otb_string+=" ";
                }
                if (!trains.empty())
                {
                    auto otb = track.GetOccupiedTrainBlocks();
                    for (int j=0;j<otb.size();j++)
                    {
                        for (int i=0;i<otb[j].size();i++)
                        {
                            otb_string+=std::to_string(otb[j][i]);
                            otb_string+=" ";
                        }
                }
                //track_model_ui->set_blocks_occupied_train(otb_string.c_str());
                }
                track_model_ui->set_blocks_occupied(otb_string.c_str());
            }
        });

        track_model_ui->on_request_update_pfail_block_tb([&]() {
            if (auto ui = weak_ui_handle.lock()) {
            std::string string_block_num = std::string(track_model_ui->get_pfail_block());
            int int_block_num = std::stoi(string_block_num);
                bool pfail_bool = track_model_ui->get_pfail_bool_tb();
                track.SetPowerFailure(int_block_num, pfail_bool);
                auto otb = track.GetOccupiedTrainBlocks();
                std::vector<std::shared_ptr<train_model::TrainModel>> trains;
                track.GetTrainModels(trains);
                std::string otb_string="";

                auto failed = track.GetFailedBlocks();
                for (int i=0;i<failed.size();i++)
                {
                    otb_string+=std::to_string(failed[i]);
                    otb_string+=" ";
                }
                if (!trains.empty())
                {
                    auto otb = track.GetOccupiedTrainBlocks();
                    for (int j=0;j<otb.size();j++)
                    {
                        for (int i=0;i<otb[j].size();i++)
                        {
                            otb_string+=std::to_string(otb[j][i]);
                            otb_string+=" ";
                        }
                    }
                }
                track_model_ui->set_blocks_occupied(otb_string.c_str());
            }
        });

        track_model_ui->on_request_update_brail_block_tb([&]() {
            if (auto ui = weak_ui_handle.lock()) {
            std::string string_block_num = std::string(track_model_ui->get_brail_block());
            int int_block_num = std::stoi(string_block_num);
                bool brail_bool = track_model_ui->get_brail_bool_tb();
                track.SetBrokenRail(int_block_num, brail_bool);
                auto otb = track.GetOccupiedTrainBlocks();
                std::vector<std::shared_ptr<train_model::TrainModel>> trains;
                track.GetTrainModels(trains);
                std::string otb_string="";

                auto failed = track.GetFailedBlocks();
                for (int i=0;i<failed.size();i++)
                {
                    otb_string+=std::to_string(failed[i]);
                    otb_string+=" ";
                }
                if (!trains.empty())
                {
                    auto otb = track.GetOccupiedTrainBlocks();
                    for (int j=0;j<otb.size();j++)
                    {
                        for (int i=0;i<otb[j].size();i++)
                        {
                            otb_string+=std::to_string(otb[j][i]);
                            otb_string+=" ";
                        }
                    }
                }
                track_model_ui->set_blocks_occupied(otb_string.c_str());
            }
        });

        track_model_ui->on_request_update_external_temp([&]() {
            if (auto ui = weak_ui_handle.lock()) {
            std::string string_external_temp = std::string(track_model_ui->get_external_temp());
            int int_external_temp = std::stoi(string_external_temp);
                track.SetExternalTemperature(int_external_temp);
            }
        }); 

        track_model_ui->on_request_block_info([&](){
                if (auto ui = weak_ui_handle.lock()) {
                    std::string string_block_num = std::string(track_model_ui->get_block_number());
                    int int_block_num = std::stoi(string_block_num);
                    types::Block requested_block;
                    track.GetBlock(int_block_num, requested_block);

                        if (requested_block.block==0)
                        {
                            track_model_ui->set_block_switch("63");
                        }
                        else if(requested_block.block==150)
                        {
                            track_model_ui->set_block_switch("28");
                        }
                        else
                        {
                            if (requested_block.switched==1)
                            {
                                std::string connection_string = std::to_string(requested_block.switch_connection);
                                track_model_ui->set_block_switch(connection_string.c_str());
                            }
                            else
                            {
                                std::string connection_string = std::to_string(requested_block.block + 1);
                                track_model_ui->set_block_switch(connection_string.c_str());
                            }
                        }

                    if (requested_block.has_crossing==1)
                    {
                        if (requested_block.crossing_state==1)
                        {
                            track_model_ui->set_block_crossing("ON");
                        }
                        else
                        {
                            track_model_ui->set_block_crossing("OFF");
                        }
                    }
                    else 
                    {
                        track_model_ui->set_block_crossing("N/A");
                    }

                    if (requested_block.has_station==1)
                    {
                        track_model_ui->set_block_station("YES");
                    }
                    else 
                    {
                        track_model_ui->set_block_station("NO");
                    }

                    std::string block_grade = std::to_string(requested_block.grade);
                    track_model_ui->set_block_grade(block_grade.substr(0, 4).c_str());

                    std::string block_elevation = std::to_string(requested_block.elevation*3.28);
                    track_model_ui->set_block_elevation(block_elevation.substr(0, 4).c_str());

                    std::string block_cum_elevation = std::to_string(requested_block.cumulative_elevation*3.28);
                    track_model_ui->set_block_cumulative_elevation(block_cum_elevation.substr(0, 4).c_str());

                    if (requested_block.underground==1)
                    {
                        track_model_ui->set_block_underground("YES");
                    }
                    else {
                        track_model_ui->set_block_underground("NO");
                    }

                    if (requested_block.track_circuit_failure==1)
                    {
                        track_model_ui->set_current_tc_fail("YES");
                    }
                    else {
                        track_model_ui->set_current_tc_fail("NO");
                    }

                    if (requested_block.power_failure==1)
                    {
                        track_model_ui->set_current_pfail("YES");
                    }
                    else {
                        track_model_ui->set_current_pfail("NO");
                    }

                    if (requested_block.broken_rail==1)
                    {
                        track_model_ui->set_current_brail("YES");
                    }
                    else {
                        track_model_ui->set_current_brail("NO");
                    }

                    if (requested_block.heater_on==1)
                    {
                        track_model_ui->set_block_heater("ON");
                    }
                    else {
                        track_model_ui->set_block_heater("OFF");
                    }

                    if (requested_block.occupied==1)
                    {
                        track_model_ui->set_block_occupied("YES");
                    }
                    else {
                        track_model_ui->set_block_occupied("NO");
                    }

                    if (requested_block.has_switch==1)
                    {
                        if (requested_block.light_color==types::TrafficLightColor::TRAFFICLIGHTCOLOR_GREEN)
                        {
                            track_model_ui->set_block_light_color("GREEN");
                        }
                        if (requested_block.light_color==types::TrafficLightColor::TRAFFICLIGHTCOLOR_RED)
                        {
                            track_model_ui->set_block_light_color("RED");
                        }
                        if (requested_block.light_color==types::TrafficLightColor::TRAFFICLIGHTCOLOR_YELLOW)
                        {
                            track_model_ui->set_block_light_color("YELLOW");
                        }
                        if (requested_block.light_color==types::TrafficLightColor::TRAFFICLIGHTCOLOR_NONE)
                        {
                            track_model_ui->set_block_light_color("OFF");
                        }
                    }
                    else 
                    {
                        track_model_ui->set_block_light_color("N/A");
                    }

                    if (requested_block.direction==types::BlockDirection::BLOCKDIRECTION_BIDIRECTIONAL)
                    {
                        track_model_ui->set_block_direction("BIDIRECTIONAL");
                    }
                    else
                    {
                        track_model_ui->set_block_direction("UNIDIRECTIONAL");
                    }

                    std::string block_length = std::to_string(requested_block.length*3.28);
                    track_model_ui->set_block_length(block_length.substr(0, 5).c_str());
                }
        });
    });
    });

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

    launcher_ui->run();
    worker_thread.join();

    return 0;
}