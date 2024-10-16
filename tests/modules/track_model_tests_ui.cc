/*****************************************************************************
 * @file track_model_tests.cc
 *
 * @brief Short-term testing for the Track Model backend.
 *****************************************************************************/

#include <thread>
#include <chrono>
#include <slint.h>
#include <random>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <sstream>
#include <vector>

#include "track_model.h"
#include "track_model_ui.h"
#include "train_model.h"
#include "csv_parser.h"
#include "types.h"
#include "channel.h"

types::Error SetTrackLayout(const std::vector<std::vector<std::string> > records, std::vector<Block> line, std::string linetype)
{
    //print records
    std::cout << "BACKEND READ A SIZE OF " << records.size() << std::endl;

    //check if the line is blue, green, or red
    if (records[1][0] == "Green")
    {
        linetype = "GREEN";
    }
    else
    if (records[1][0] == "Red")
    {
        linetype = "RED";
    }
    else{
        linetype = "BLUE";
    }

    //making the line the size of the track input
    line.resize(records.size() + 1);

    //printing line type
    std::cout << "LINE IS " << records[1][0] << std::endl;

    //parsing records
    for (int i = 1; i < records.size(); i++)
    {
        if (records[i][1] != " ")
        {
            line[i].section = records[i][1];
            //std::cout << line[i-1].section << std::endl;
        }
        if (records[i][2] != " ")
        {
            line[i].blocknum = stoi(records[i][2]);
            //std::cout << line[i-1].blocknum << std::endl;
        }
        if (records[i][3] != " ")
        {
            line[i].length = stoi(records[i][3]);
            //std::cout << line[i-1].length << std::endl;
        }
        if (records[i][4] != " ")
        {
            line[i].grade = stof(records[i][4]);
            //std::cout << line[i-1].grade << std::endl;
        }
        if (records[i][5] != " ")
        {
            line[i].slimit = stoi(records[i][5]);
            //std::cout << line[i-1].slimit << std::endl;
        }
        if (records[i][6] != " ")
        {
            std::string temp = records[i][6];
            if (temp.substr(0, 7) == "STATION")
            {
                line[i].station = 1;
                //std::cout << "STATION AT BLOCK " << i << std::endl;
            }
            else
            if (temp.substr(0, 7) == "RAILWAY")
            {
                line[i].crossing = 1;
                //std::cout << "CROSSING AT BLOCK " << i << std::endl;;
            }
            else
            if (temp.substr(0, 11) == "UNDERGROUND")
            {
                line[i].underground = 1;
                //std::cout << "UNDERGROUND FOR BLOCK " << i << " = " << line[i].underground << std::endl;
            }
            else
            if (temp.substr(0, 6) == "SWITCH")
            {
                //CHECK IF IT IS TO THE YARD
                if (temp.substr(7, 1) == "T")
                {
                    line[i - 1].Switch = 0;
                    continue;
                }
                else
                if (temp.substr(7, 1) == "F")
                {
                    line[i + 1].Switch = 0;
                    continue;
                }
                //find ";" and "-"
                size_t semicolonpos = 0;
                size_t dashpos      = 0;
                semicolonpos = temp.find(';', semicolonpos);
                dashpos      = temp.find('-', semicolonpos);
                //find ")"
                size_t closep = 0;
                closep = temp.find(')', dashpos);

                std::string segment = temp.substr(semicolonpos + 2, dashpos - semicolonpos - 2);
                std::cout << segment << std::endl;
                if (segment == records[i][2]) //if first number is the current block
                {
                    segment        = temp.substr(dashpos + 1, closep - dashpos - 1);
                    line[i].Switch = stoi(segment);
                    //std::cout << "SWITCH FOR BLOCK " << i << " = " << line[i].Switch << " TYPE 1" << std::endl ;
                }
                else
                if (stoi(segment) == (stoi(records[i][2]) + 1))  //if first number is the next block
                {
                    segment            = temp.substr(dashpos + 1, closep - dashpos - 1);
                    line[i + 1].Switch = (stoi(segment));
                    //std::cout << "SWITCH FOR BLOCK " << i+1 << " = " << line[i+1].Switch << " TYPE 2" << std::endl;
                }
                else
                {
                    std::string segment2 = temp.substr(dashpos + 1, closep - dashpos - 1);
                    //std::cout << "SEGMENT 2 IS " << segment2 << std::endl;
                    if (segment2 == records[i][2]) //second number is current block
                    {
                        line[i].Switch = (stoi(segment));
                        //std::cout << "SWITCH FOR BLOCK " << i << " = " << line[i].Switch << " TYPE 3" << std::endl;
                    }
                    else //second number is next block
                    {
                        line[i + 1].Switch = (stoi(segment));
                        //std::cout << "SWITCH FOR BLOCK " << i+1 << " = " << line[i+1].Switch << " TYPE 4" << std::endl;
                    }
                }
            }
        }
        if (records[i][8] != " ")
        {
            line[i].elevation = stof(records[i][8]);
            //std::cout << "ELEVATION FOR BLOCK " << i << " IS " << line[i].elevation << std::endl;
        }
        if (records[i][9] != " ")
        {
            line[i].cumelevation = stof(records[i][9]);
            //std::cout << "CUMULATIVE ELEVATION FOR BLOCK " << i << " IS " << line[i].cumelevation << std::endl;
        }
        if (records[i][10] != " " && linetype == "GREEN")
        {
            line[i].sectotraverse = stof(records[i][10]);
            //std::cout << "TIME TO TRAVERSE FOR BLOCK " << i << " IS " << line[i].sectotraverse << std::endl;;
        }
    }
}

types::Error SetTrainBlock(const types::BlockId block, std::vector<Block> line, types::BlockId currblock)
{
    //checking if block exists
    if (line.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    currblock = block;

    return types::ERROR_NONE;
}

void Update(int externaltemp, std::vector<Block> line, int currblock, uint16_t deboarding, uint16_t boarding, uint16_t tpassengers, std::vector<types::BlockId> trainblockvec)
{
    // Update the state of the track model

    //check temperature for heaters
    if (externaltemp <= 32)
    {
        //set all track heaters
        for (int i = 0; i < line.size(); i++)
        {
            line[i].heater = 1;
        }
    }

    //check if train is at station
    if (line[currblock].station == 1)
    {
        //update the passengers on board!
        tpassengers -= deboarding;
        tpassengers += boarding;
        boarding     = 0;
        deboarding   = 0;
    }

    //clear old occupancies
    for (int k = 0; k < trainblockvec.size(); k++)
    {
        int pos = trainblockvec[k];
        line[pos].occupancy = 0;
    }

    //get the BlockId of the train
    types::BlockId trainblock = currblock;

    //set block occupancy
    line[trainblock].occupancy = 1;

    trainblockvec.push_back(trainblock);

    //calculate which blocks the train is currently taking up
    //check the length of the block the train is occupying, and see if the train is longer
    if (line[trainblock].length < 43)
    {
        //looping until the full length of the train is accounted for
        auto sizeofblocks = line[trainblock].length;
        int  j            = trainblock - 1;
        while (sizeofblocks < 32)
        {
            //checking if the block behind this is connected to another block
            if (line[j].connection != 0)
            {
                //adding size of block connected
                types::BlockId blockcon = line[j].connection;

                //adjust j
                j = blockcon;
            }
            //adding the size of the block behind it
            sizeofblocks += line[j - 1].length;

            //adding this block to the vector of occupancies
            line[j].occupancy = 1;

            //adding to current train blocks
            trainblockvec.push_back(j);

            //increment
            j--;
            ;
        }
    }
}

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
    //temps
    std::vector<Block> line;
    types::TrackId track_id = 1;
    std::vector<std::shared_ptr<train_model::TrainModel> > trainsvec;
    std::vector<types::BlockId> trainblockvec;   //make vector for multiple trains
    uint16_t tpassengers                  = 150; //make vector for multiple trains
    uint16_t boarding                     = 0;   //make vector for multiple trains
    uint16_t deboarding                   = 0;
    int externaltemp = 50;
    //vector containing current actual line with swtiches and branches factored in
    int maxpass = 222;
    std::vector<std::shared_ptr<train_model::TrainModel> > trainmodels;
    types::Meters tlength = 32;
    std::string linetype = "BLUE";
    types::BlockId currblock = 0;


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
    Channel<std::string> brailchannel;
    Channel<bool> brailboolchannel;
    Channel<std::string> tcfailchannel;
    Channel<bool> tcfailboolchannel;
    Channel<std::string> pfailchannel;
    Channel<bool> pfailboolchannel;
    Channel<std::vector<std::string>> occupiedchannel;
    Channel<std::string> boardchannel;
    Channel<std::vector<std::vector<std::string>>> recordschannel;
    Channel<std::string> numblockschannel;

    /*track_model_ui->on_request_update_external_temp([&](){
            tempchannel.Send(std::string(track_model_ui->get_external_temp()));
            externaltemp=(stoi(tempchannel.Receive()));
            std::string externaltempstring=std::to_string(externaltemp);
            track_model_ui->set_external_temp(externaltempstring.c_str());
            Update(externaltemp, line, currblock, deboarding, boarding, tpassengers, trainblockvec);
    });*/
    
    track_model_ui->on_request_update_train_block([&](){
            blockidchannel.Send(std::string(track_model_ui->get_train_block()));
            currblock=(stoi(blockidchannel.Receive()));

            //check temperature for heaters
            if (externaltemp <= 32)
            {
                //set all track heaters
                for (int i = 0; i < line.size(); i++)
                {
                    line[i].heater = 1;
                }
            }

            //check if train is at station
            if (line[currblock].station == 1)
            {
                //update the passengers on board!
                tpassengers -= deboarding;
                tpassengers += boarding;
                boarding     = 0;
                deboarding   = 0;
            }

            //clear old occupancies
            for (int k = 0; k < trainblockvec.size(); k++)
            {
                int pos = trainblockvec[k];
                line[pos].occupancy = 0;
            }

            //get the BlockId of the train
            types::BlockId trainblock = currblock;

            //set block occupancy
            line[trainblock].occupancy = 1;

            trainblockvec.push_back(trainblock);

            //calculate which blocks the train is currently taking up
            //check the length of the block the train is occupying, and see if the train is longer
            if (line[trainblock].length < 43)
            {
                //looping until the full length of the train is accounted for
                auto sizeofblocks = line[trainblock].length;
                int  j            = trainblock - 1;
                while (sizeofblocks < 32)
                {
                    //checking if the block behind this is connected to another block
                    if (line[j].connection != 0)
                    {
                        //adding size of block connected
                        types::BlockId blockcon = line[j].connection;

                        //adjust j
                        j = blockcon;
                    }
                    //adding the size of the block behind it
                    sizeofblocks += line[j].length;

                    //adding this block to the vector of occupancies
                    line[j].occupancy = 1;

                    //adding to current train blocks
                    trainblockvec.push_back(j);

                    //increment
                    j--;
                }
            }

            std::string occupancystring="";

            for (int i=0;i<line.size();i++)
            {
                if (line[i].occupancy==1)
                {
                    occupancystring+=std::to_string(line[i].blocknum)+" ";
                }
            }

            track_model_ui->set_blocks_occupied(occupancystring.c_str());
    });

    track_model_ui->on_request_update_passengers_deboarding([&](){
            //deboardchannel.Send(std::string(track_model_ui->get_passengers_deboarding()));
            deboardchannel.Send(std::string(track_model_ui->get_passengers_deboarding()));

            //get the passengers deboarding
            deboarding = stoi(deboardchannel.Receive());

            //subtract from total passengers
            uint16_t vacancy = maxpass - tpassengers + deboarding;

            //generate random number within bounds for boarding
            std::random_device rd;  // Seed
            std::mt19937 gen(rd());  // Mersenne Twister engine
            std::uniform_int_distribution<> dis(0, vacancy); // Uniform distribution between 0 and board

            // Generate a random number
            int randomNumber = dis(gen);

            //set passengers boarding
            boarding = randomNumber;
            //boardchannel.Send(std::to_string(boarding));
            track_model_ui->set_passengers_boarding((std::to_string(boarding)).c_str()); //access ui. do ui.get_train_block for example
    });

    /*

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
    */

    track_model_ui->on_request_update_brail_block([&](){
            brailchannel.Send(std::string(track_model_ui->get_brail_block()));
            brailboolchannel.Send(track_model_ui->get_brail_bool());
            int brailblockint=stoi(brailchannel.Receive());
            bool brailbooltemp=brailboolchannel.Receive();

            if (line.size() >= brailblockint && brailblockint>0)
            {
                //setting broken rail
                line[brailblockint].brail = brailbooltemp;
                line[brailblockint].occupancy = brailbooltemp;
            }

            std::string occupancystring="";

            for (int i=0;i<line.size();i++)
            {
                if (line[i].occupancy==1)
                {
                    occupancystring+=std::to_string(line[i].blocknum)+" ";
                }
            }

            track_model_ui->set_blocks_occupied(occupancystring.c_str());
            
            //trackModel.Update();
    });

    track_model_ui->on_request_update_pfail_block([&](){
            pfailchannel.Send(std::string(track_model_ui->get_pfail_block()));
            pfailboolchannel.Send(track_model_ui->get_pfail_bool());
            int pfailblock=stoi(pfailchannel.Receive());
            bool pfailbool=pfailboolchannel.Receive();

            if (line.size() >= pfailblock && pfailblock>0)
            {
                //setting broken rail
                line[pfailblock].brail = pfailbool;
                line[pfailblock].occupancy = pfailbool;
            }

            std::string occupancystring="";

            for (int i=0;i<line.size();i++)
            {
                if (line[i].occupancy==1)
                {
                    occupancystring+=std::to_string(line[i].blocknum)+" ";
                }
            }

            track_model_ui->set_blocks_occupied(occupancystring.c_str());
            //trackModel.Update();
    });

    /*

    track_model_ui->on_request_update_tcfail_block([&](){
            tcfailchannel.Send(track_model_ui->get_tcfail_block());
    });

    track_model_ui->on_request_block_info([&](){
            requestchannel.Send(std::string(track_model_ui->get_view_block()));
            //trackModel.Update();
    }); */

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
            //recordschannel.Send(records);
            //trackModel.SetTrackLayout(recordschannel.Receive()); //turn this to a set function. Define recordschannel earlier and use it here. 
            //SetTrackLayout(records, line, linetype);

                //print records
                std::cout << "BACKEND READ A SIZE OF " << records.size() << std::endl;

                //check if the line is blue, green, or red
                if (records[1][0] == "Green")
                {
                    linetype = "GREEN";
                }
                else
                if (records[1][0] == "Red")
                {
                    linetype = "RED";
                }
                else{
                    linetype = "BLUE";
                }

                //making the line the size of the track input
                line.resize(records.size() + 1);

                //printing line type
                std::cout << "LINE IS " << records[1][0] << std::endl;

                //parsing records
                for (int i = 1; i < records.size(); i++)
                {
                    if (records[i][1] != " ")
                    {
                        line[i].section = records[i][1];
                        //std::cout << line[i-1].section << std::endl;
                    }
                    if (records[i][2] != " ")
                    {
                        line[i].blocknum = stoi(records[i][2]);
                        //std::cout << line[i-1].blocknum << std::endl;
                    }
                    if (records[i][3] != " ")
                    {
                        line[i].length = stoi(records[i][3]);
                        //std::cout << line[i-1].length << std::endl;
                    }
                    if (records[i][4] != " ")
                    {
                        line[i].grade = stof(records[i][4]);
                        //std::cout << line[i-1].grade << std::endl;
                    }
                    if (records[i][5] != " ")
                    {
                        line[i].slimit = stoi(records[i][5]);
                        //std::cout << line[i-1].slimit << std::endl;
                    }
                    if (records[i][6] != " ")
                    {
                        std::string temp = records[i][6];
                        if (temp.substr(0, 7) == "STATION")
                        {
                            line[i].station = 1;
                            //std::cout << "STATION AT BLOCK " << i << std::endl;
                        }
                        else
                        if (temp.substr(0, 7) == "RAILWAY")
                        {
                            line[i].crossing = 1;
                            //std::cout << "CROSSING AT BLOCK " << i << std::endl;;
                        }
                        else
                        if (temp.substr(0, 11) == "UNDERGROUND")
                        {
                            line[i].underground = 1;
                            //std::cout << "UNDERGROUND FOR BLOCK " << i << " = " << line[i].underground << std::endl;
                        }
                        else
                        if (temp.substr(0, 6) == "SWITCH")
                        {
                            //CHECK IF IT IS TO THE YARD
                            if (temp.substr(7, 1) == "T")
                            {
                                line[i - 1].Switch = 0;
                                continue;
                            }
                            else
                            if (temp.substr(7, 1) == "F")
                            {
                                line[i + 1].Switch = 0;
                                continue;
                            }
                            //find ";" and "-"
                            size_t semicolonpos = 0;
                            size_t dashpos      = 0;
                            semicolonpos = temp.find(';', semicolonpos);
                            dashpos      = temp.find('-', semicolonpos);
                            //find ")"
                            size_t closep = 0;
                            closep = temp.find(')', dashpos);

                            std::string segment = temp.substr(semicolonpos + 2, dashpos - semicolonpos - 2);
                            std::cout << segment << std::endl;
                            if (segment == records[i][2]) //if first number is the current block
                            {
                                segment        = temp.substr(dashpos + 1, closep - dashpos - 1);
                                line[i].Switch = stoi(segment);
                                //std::cout << "SWITCH FOR BLOCK " << i << " = " << line[i].Switch << " TYPE 1" << std::endl ;
                            }
                            else
                            if (stoi(segment) == (stoi(records[i][2]) + 1))  //if first number is the next block
                            {
                                segment            = temp.substr(dashpos + 1, closep - dashpos - 1);
                                line[i + 1].Switch = (stoi(segment));
                                //std::cout << "SWITCH FOR BLOCK " << i+1 << " = " << line[i+1].Switch << " TYPE 2" << std::endl;
                            }
                            else
                            {
                                std::string segment2 = temp.substr(dashpos + 1, closep - dashpos - 1);
                                //std::cout << "SEGMENT 2 IS " << segment2 << std::endl;
                                if (segment2 == records[i][2]) //second number is current block
                                {
                                    line[i].Switch = (stoi(segment));
                                    //std::cout << "SWITCH FOR BLOCK " << i << " = " << line[i].Switch << " TYPE 3" << std::endl;
                                }
                                else //second number is next block
                                {
                                    line[i + 1].Switch = (stoi(segment));
                                    //std::cout << "SWITCH FOR BLOCK " << i+1 << " = " << line[i+1].Switch << " TYPE 4" << std::endl;
                                }
                            }
                        }
                    }
                    if (records[i][8] != " ")
                    {
                        line[i].elevation = stof(records[i][8]);
                        //std::cout << "ELEVATION FOR BLOCK " << i << " IS " << line[i].elevation << std::endl;
                    }
                    if (records[i][9] != " ")
                    {
                        line[i].cumelevation = stof(records[i][9]);
                        //std::cout << "CUMULATIVE ELEVATION FOR BLOCK " << i << " IS " << line[i].cumelevation << std::endl;
                    }
                    if (records[i][10] != " " && linetype == "GREEN")
                    {
                        line[i].sectotraverse = stof(records[i][10]);
                        //std::cout << "TIME TO TRAVERSE FOR BLOCK " << i << " IS " << line[i].sectotraverse << std::endl;;
                    }
                }
                int thenumblocks=records.size();
                track_model_ui->set_num_blocks(std::to_string(thenumblocks).c_str());
            }); 

    });

    /*slint::ComponentWeakHandle<ui::TrackModelUi> weak_tkm_handle(track_model_ui);
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
    //worker_thread.join();

    return 0;
}