#include "track_model.h"
#include "train_model.h"

namespace track_model
{

    //cmake --build build -t target
    //BLUE LINE CONSTRUCTOR
    track_model::SoftwareTrackModel::SoftwareTrackModel(){
        //if nothing, blue line!
        track_id=1;
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

        newstuff[2].crossing=1;
        newstuff[4].switches.push_back(6);
        newstuff[4].switches.push_back(11);
        newstuff[5].light=1;
        newstuff[9].station='B';
        newstuff[10].light=1;
        newstuff[14].station='C';

        for (int i=5;i<10;i++)
        {
            newstuff[i].section='B';
        }

        for (int i=11;i<15;i++)
        {
            newstuff[i].section='C';
        }

        line.insert(line.end(),newstuff.begin(),newstuff.end());
    }

    /*types::TrackId SoftwareTrackModel::GetTrackId(void) {
    // Implementation logic to get the Track ID
    // Return a dummy TrackId for demonstration
    return types::TrackId{ };
    }

    */

    types::Error SoftwareTrackModel::AddTrainModel(std::shared_ptr<train_model::TrainModel> train) {
        // Implementation logic to add a train model

        //TODO: ADD FAILURE STATES
        
        //add model
        trainmodels.push_back(train);
        currblock.push_back(1);
        
        return types::ERROR_NONE;
    }


    void SoftwareTrackModel::GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel>> &trains) const {
        // Populating the trains vector with current train models
        for (int i=0;i<trainmodels.size();i++)
        {
            trains.push_back(trainmodels[i]);
        }
    }
    

    void SoftwareTrackModel::Update(void) {
        // Update the state of the track model

        //loop through all trains on the line
        for (int i=0;i<trainmodels.size();i++)
        {
            //get the BlockId of the train
            types::BlockId trainblock;

            //set block occupancy
            line[trainblock-1].occupancy=1;

            //calculate which blocks the train is currently taking up
            //check the length of the block the train is occupying, and see if the train is longer
            if (line[trainblock-1].length<tlength)
            {
                //looping until the full length of the train is accounted for
                auto sizeofblocks=line[trainblock-1].length;
                int j=1;
                while (sizeofblocks<tlength)
                {
                    //adding the size of the block behind it
                    sizeofblocks+=line[trainblock-1-j].length;

                    //adding this block to the vector of occupancies
                    line[trainblock-1-j].occupancy=1;

                    //increment
                    j++;
                }
            }

            //get the passengers deboarding
            uint16_t deboard=trainmodels[i]->GetPassengersDeboarding();

            //subtract from total passengers
            tpassengers[i]-=deboard;
            uint16_t vacancy=maxpass-tpassengers[i];

            //generate random number within bounds for boarding
            srand((unsigned)time(0));
            uint16_t board;
            board=(rand()%vacancy);

            //set passengers boarding
            trainmodels[i]->SetPassengersBoarding(board);
        }
        

    }
    /*

    types::Error SoftwareTrackModel::SetSwitchState(const types::BlockId block, const bool switched) {
        // Logic to set the switch state for the specified block
        return types::Error{ };
    }

    types::Error SoftwareTrackModel::SetCrossingState(const types::BlockId block, const bool closed) {
        // Logic to set the crossing state for the specified block
        return types::Error{ };
    }

    types::Error SoftwareTrackModel::SetRedTrafficLight(const types::BlockId block, const bool on) {
        // Logic to set the red traffic light state for the specified block
        return types::Error{ };
    }

    types::Error SoftwareTrackModel::SetYelloTrafficLight(const types::BlockId block, const bool on) {
        // Logic to set the yellow traffic light state for the specified block
        return types::Error{ };
    }

    types::Error SoftwareTrackModel::SetGreenTrafficLight(const types::BlockId block, const bool on) {
        // Logic to set the green traffic light state for the specified block
        return types::Error{ };
    }

    types::Error SoftwareTrackModel::SetCommandedSpeed(const types::BlockId block, const types::MetersPerSecond speed) {
        // Logic to set the commanded speed
        return types::Error{ };
    }

    types::Error SoftwareTrackModel::SetAuthority(const types::BlockId block, const types::Meters authority) {
        // Logic to set authority
        return types::Error{  };
    }*/

    types::Error SoftwareTrackModel::GetBlockOccupancy(const types::BlockId block, bool &occupied) const {
        // Logic to determine block occupancy and set the occupied variable

        //checking if block exists
        if (line.size()<block)
        {
            return types::ERROR_INVALID_BLOCK;
        }

        //check occupancy
        if (line[block-1].occupancy==1)
        {
            //set occupancy variable
            occupied=1;
        }
        else
        {
            occupied=0;
        }

        return types::ERROR_NONE;
    }

    types::Error SoftwareTrackModel::SetBrokenRail(const types::BlockId block, const bool broken) {
        // Logic to set the broken rail state for the specified block

        //checking if block exists
        if (line.size()<block)
        {
            return (types::ERROR_INVALID_BLOCK);
        }

        //setting broken rail
        line[block-1].brail=broken;

        //occupancy
        line[block-1].occupancy=1;

        return types::ERROR_NONE;
    }

    types::Error SoftwareTrackModel::SetTrackCircuitFailure(const types::BlockId block, const bool track_circuit_failure) {
        // Logic to set the track circuit failure state for the specified block
        
        //checking if block exists
        if (line.size()<block)
        {
            return (types::ERROR_INVALID_BLOCK);
        }

        //setting TC fail
        line[block-1].tcfail=track_circuit_failure;

        return types::ERROR_NONE;
    }

    types::Error SoftwareTrackModel::SetPowerFailure(const types::BlockId block, const bool power_failure) {
        // Logic to set the power failure state for the specified block
        
        //checking if block exists
        if (line.size()<block)
        {
            return (types::ERROR_INVALID_BLOCK);
        }

        //setting TC fail
        line[block-1].pfail=power_failure;

        //occupancy
        line[block-1].occupancy=1;

        return types::ERROR_NONE;
    }

} // namespace track_model