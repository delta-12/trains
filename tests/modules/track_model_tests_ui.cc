/*****************************************************************************
 * @file track_model_tests.cc
 *
 * @brief Short-term testing for the Track Model backend.
 *****************************************************************************/

#include <thread>
#include <chrono>
#include <slint.h>

#include "track_model.h"
#include "track_model_ui.h"
#include "train_model.h"
#include "csv_parser.h"
#include "types.h"
#include "channel.h"
#include <iostream>
#include <fstream>

#include <filesystem>
#include <string>
#include <sstream>
#include <vector>

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
    track_model::SoftwareTrackModel trackModel;
    auto track_model_ui = ui::TrackModelUi::create();
    std::string input_file_path="";
    struct::Block specificblock;

    Channel<std::string> blockidchannel;
    Channel<std::string> tempchannel; //move these outside of threads
    Channel<std::string> requestchannel;
    Channel<std::string> deboardchannel;
    Channel<int> switchchannel;
    Channel<bool> switchboolchannel;
    Channel<int> crossingchannel;
    Channel<bool> crossingboolchannel;
    Channel<int> greenchannel;
    Channel<int> yellowchannel;
    Channel<int> redchannel;
    Channel<int> brailchannel;
    Channel<bool> brailboolchannel;
    Channel<int> tcfailchannel;
    Channel<bool> tcfailboolchannel;
    Channel<int> pfailchannel;
    Channel<bool> pfailboolchannel;
    Channel<std::vector<std::string>> occupiedchannel;
    Channel<std::string> boardchannel;
    Channel<std::vector<std::vector<std::string>>> recordschannel;

    track_model_ui->on_request_update_external_temp([&](){
            tempchannel.Send(std::string(track_model_ui->get_external_temp()));
    });

    track_model_ui->on_request_update_train_block([&](){
            blockidchannel.Send(std::string(track_model_ui->get_train_block()));
    });

    track_model_ui->on_request_update_passengers_deboarding([&](){
            deboardchannel.Send(std::string(track_model_ui->get_passengers_deboarding()));
    });

    track_model_ui->on_request_update_switch_block([&](){
            switchchannel.Send(track_model_ui->get_switch_block());
           // trackModel.Update();
    });

    track_model_ui->on_request_update_crossing_block([&](){
            crossingchannel.Send(track_model_ui->get_crossing_block());
            //trackModel.Update();
    });

    track_model_ui->on_request_update_green_block([&](){
            greenchannel.Send(track_model_ui->get_green_block());
            //trackModel.Update();
    });

    track_model_ui->on_request_update_yellow_block([&](){
            yellowchannel.Send(track_model_ui->get_yellow_block());
    });

    track_model_ui->on_request_update_red_block([&](){
            redchannel.Send(track_model_ui->get_red_block());
    });

    track_model_ui->on_request_update_brail_block([&](){
            brailchannel.Send(track_model_ui->get_brail_block());
            //trackModel.Update();
    });

    track_model_ui->on_request_update_tcfail_block([&](){
            tcfailchannel.Send(track_model_ui->get_tcfail_block());
    });

    track_model_ui->on_request_update_pfail_block([&](){
            pfailchannel.Send(track_model_ui->get_pfail_block());
            //trackModel.Update();
    });

    track_model_ui->on_request_block_info([&](){
            requestchannel.Send(std::string(track_model_ui->get_view_block()));
            //trackModel.Update();
    });

    track_model_ui->on_choose_file([&]() {
        // For other platforms
        std::string path;
        std::cout << "Please enter the path to the CSV file: ";

        std::getline(std::cin, input_file_path);

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

            std::filesystem::path path(input_file_path);
            CsvParser parser(path);

            std::vector<std::vector<std::string> > records = parser.GetRecords();
            std::cout << "Track size: " << parser.GetSize() << std::endl;
            for(const std::vector<std::string> &record : records)
            {
                for(const std::string &value : record)
                {
                    std::cout << value << ", ";
                }
                std::cout << "Record Size: " << record.size();
                std::cout << std::endl;
            }
            recordschannel.Send(records);
            trackModel.SetTrackLayout(recordschannel.Receive()); //turn this to a set function. Define recordschannel earlier and use it here. 
        });    
    });

    slint::ComponentWeakHandle<ui::TrackModelUi> weak_tkm_handle(track_model_ui);
    std::thread worker_thread([&]
    {
        //EXTERNAL TEMP
        //trackModel.SetExternalTemperature(stoi(tempchannel.Receive()));
        //trackModel.Update();

        //trainblock
        trackModel.SetTrainBlock(stoi(blockidchannel.Receive()));
        trackModel.Update();

        //DEBOARD
        trackModel.SetPassengersDeboarding(stoi(deboardchannel.Receive()));
        std::cout << "receive successful" << std::endl;
        //trackModel.Update();
        boardchannel.Send(std::to_string(trackModel.GetPassengersBoarding()));
        /*
        
        //switches
        trackModel.SetSwitchState(switchchannel.Receive(),track_model_ui->get_switch_bool());

        //crossing
        trackModel.SetCrossingState(crossingchannel.Receive(),track_model_ui->get_crossing_bool());

        //green light
        trackModel.SetGreenTrafficLight(greenchannel.Receive(),track_model_ui->get_green_bool());

        //yellow light
        trackModel.SetYelloTrafficLight(yellowchannel.Receive(),track_model_ui->get_yellow_bool());

        //red
        trackModel.SetRedTrafficLight(redchannel.Receive(),track_model_ui->get_red_bool());

        //broken rail
        trackModel.SetBrokenRail(brailchannel.Receive(),track_model_ui->get_brail_bool());

        //tc fail
        trackModel.SetTrackCircuitFailure(tcfailchannel.Receive(),track_model_ui->get_tcfail_bool());

        //power fail
        trackModel.SetPowerFailure(pfailchannel.Receive(),track_model_ui->get_pfail_bool());

        //block to biew
        specificblock=trackModel.GetSpecificBlock(stoi(requestchannel.Receive()));
        */
        slint::invoke_from_event_loop([weak_tkm_handle, &boardchannel, &blockidchannel, &occupiedchannel]() { //weak pointer, no ownership. can read data from it but cant mutate. USE THIS FOR BACKEND TO FRONTEND SENDING SPECIFICALLY. FRONTEND TO BACKEND SHOULD BE DONE IN THE UI THREAD
        if (auto ui = weak_tkm_handle.lock()) { //creating smart pointer
            if (ui.has_value()) //ensuring the thing it is pointing to still exists. creates shared pointer.
            {
                    ui.value()->set_passengers_boarding(boardchannel.Receive().c_str()); //access ui. do ui.get_train_block for example

                    ui.value()->set_train_block(blockidchannel.Receive().c_str());

                    std::string occupystring="";
                    for (int i=0;i<(occupiedchannel.Receive()).size();i++)
                    {
                        occupystring=occupystring+(occupiedchannel.Receive())[i]+", ";
                    }
                //ui.value()->set_blocks_occupied(occupystring);

                //MAIN UI VALUES
                //ui.value()->set_selected_block_occupancy(selectedoccupancy)
            }
        }
        });
        //std::this_thread::sleep_for(std::chrono::seconds(1));
    }); //combo box for drop down*/

    track_model_ui->run();
    worker_thread.join();

    return 0;
}