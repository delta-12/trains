#include "track_model.h"
#include "train_model.h"
#include <iostream>
#include <string>

namespace track_model
{
//BLUE LINE CONSTRUCTOR
track_model::SoftwareTrackModel::SoftwareTrackModel()
{
    track_id = 1;
    std::vector<Block> newstuff = {
        Block(),
        Block(),
        Block(),
        Block(),
        Block(),
        Block(),
        Block(),
        Block(),
        Block(),
        Block(),
        Block(),
        Block(),
        Block(),
        Block(),
        Block(),
        Block()
    };

    newstuff[2].crossing = 1;
    newstuff[4].Switch   = 6;
    newstuff[4].Switch   = 11;
    newstuff[5].light    = 1;
    newstuff[9].station  = 'B';
    newstuff[10].light   = 1;
    newstuff[14].station = 'C';

    for (int i = 5; i < 10; i++)
    {
        newstuff[i].section = 'B';
    }

    for (int i = 11; i < 15; i++)
    {
        newstuff[i].section = 'C';
    }

    line.insert(line.end(), newstuff.begin(), newstuff.end());
}

//constructor for when a track is passsed in
track_model::SoftwareTrackModel::SoftwareTrackModel(const std::vector<std::vector<std::string> > records)
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
                    std::cout << "SEGMENT 2 IS " << segment2 << std::endl;
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

types::TrackId SoftwareTrackModel::GetTrackId(void)
{
    // Implementation logic to get the Track ID
    return track_id;
}

types::Error SoftwareTrackModel::AddTrainModel(std::shared_ptr<train_model::TrainModel> train)
{
    // Implementation logic to add a train model

    //TODO: ADD FAILURE STATES

    //add model
    trainmodels.push_back(train);

    return types::ERROR_NONE;
}


void SoftwareTrackModel::GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel> > &trains) const
{
    // Populating the trains vector with current train models
    for (int i = 0; i < trainmodels.size(); i++)
    {
        trains.push_back(trainmodels[i]);
    }
}


void SoftwareTrackModel::Update(void)
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

    //loop through all trains on the line
    for (int i = 0; i < trainmodels.size(); i++)
    {
        //clear old occupancies
        for (int k = 0; k < trainblockvec.size(); k++)
        {
            int pos = trainblockvec[i][k];
            line[pos].occupancy = 0;
        }

        //get the BlockId of the train
        types::BlockId trainblock = trainmodels[i]->GetBlockId();

        //set block occupancy
        line[trainblock].occupancy = 1;

        trainblockvec[i].push_back(trainblock);

        //calculate which blocks the train is currently taking up
        //check the length of the block the train is occupying, and see if the train is longer
        if (line[trainblock].length < tlength)
        {
            //looping until the full length of the train is accounted for
            auto sizeofblocks = line[trainblock].length;
            int  j            = trainblock - 1;
            while (sizeofblocks < tlength)
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
                trainblockvec[i].push_back(j);

                //increment
                j--;
                ;
            }
        }

        //get the passengers deboarding
        uint16_t deboard = trainmodels[i]->GetPassengersDeboarding();

        //subtract from total passengers
        tpassengers[i] -= deboard;
        uint16_t vacancy = maxpass - tpassengers[i];

        //generate random number within bounds for boarding
        srand((unsigned)time(0));
        uint16_t board;
        board = (rand() % vacancy);

        //set passengers boarding
        trainmodels[i]->SetPassengersBoarding(board);
    }


}


types::Error SoftwareTrackModel::SetSwitchState(const types::BlockId block, const bool Switched)
{
    // Logic to set the Switch state for the specified block
    if (line.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    if (Switched == 1)
    {
        //find Switch states
        types::BlockId state = line[block - 1].Switch;
        line[block].connection = state;
    }
    else
    {
        line[block].connection = block + 1;
    }

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetCrossingState(const types::BlockId block, const bool closed)
{
    // Logic to set the crossing state for the specified block
    return types::Error{ };
}

types::Error SoftwareTrackModel::SetRedTrafficLight(const types::BlockId block, const bool on)
{
    // Logic to set the red traffic light state for the specified block
    if (line.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    line[block].light = "RED";

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetYelloTrafficLight(const types::BlockId block, const bool on)
{
    // Logic to set the yellow traffic light state for the specified block
    if (line.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    line[block].light = "YELLOW";

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetGreenTrafficLight(const types::BlockId block, const bool on)
{
    // Logic to set the green traffic light state for the specified block
    if (line.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    line[block].light = "GREEN";

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetCommandedSpeed(const types::BlockId block, const types::MetersPerSecond speed)
{
    // Logic to set the commanded speed
    return types::Error{ };
}

types::Error SoftwareTrackModel::SetAuthority(const types::BlockId block, const types::Meters authority)
{
    // Logic to set authority
    return types::Error{  };
}

types::Error SoftwareTrackModel::GetBlockOccupancy(const types::BlockId block, bool &occupied) const
{
    // Logic to determine block occupancy and set the occupied variable

    //checking if block exists
    if (line.size() < block)
    {
        return types::ERROR_INVALID_BLOCK;
    }
    if (block <= 0)
    {
        return types::ERROR_INVALID_BLOCK;
    }

    //check occupancy
    if (line[block].occupancy == 1)
    {
        //set occupancy variable
        occupied = 1;
    }
    else
    {
        occupied = 0;
    }

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetBrokenRail(const types::BlockId block, const bool broken)
{
    // Logic to set the broken rail state for the specified block

    //checking if block exists
    if (line.size() < block)
    {
        return (types::ERROR_INVALID_BLOCK);
    }

    //setting broken rail
    line[block].brail = broken;

    //occupancy
    line[block].occupancy = broken;

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetTrackCircuitFailure(const types::BlockId block, const bool track_circuit_failure)
{
    // Logic to set the track circuit failure state for the specified block

    //checking if block exists
    if (line.size() < block)
    {
        return (types::ERROR_INVALID_BLOCK);
    }

    //setting TC fail
    line[block].tcfail = track_circuit_failure;

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetPowerFailure(const types::BlockId block, const bool power_failure)
{
    // Logic to set the power failure state for the specified block

    //checking if block exists
    if (line.size() < block)
    {
        return (types::ERROR_INVALID_BLOCK);
    }

    //setting TC fail
    line[block].pfail = power_failure;

    //occupancy
    line[block].occupancy = power_failure;

    return types::ERROR_NONE;
}

types::Error SoftwareTrackModel::SetExternalTemperature(const types::DegreesFahrenheit temperature)
{
    externaltemp = temperature;
}

} // namespace track_model