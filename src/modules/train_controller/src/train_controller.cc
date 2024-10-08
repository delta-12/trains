#include "train_controller.h"
#include "train_model.h"
#include "types.h"

namespace train_controller
{
    types::Watts SoftwareTrainController::GetCommandedPower(void) const{

        //Returning Commanded Power
        return CommandedPower_;
    }



    void SoftwareTrainController::SetKP(uint16_t kp){
        // proportional gain
        Kp_ = kp;
    }


    void SoftwareTrainController::setKI(uint16_t ki){
        // integral gain
        Ki_ = ki;
    }


    void SoftwareTrainController::SetCommandedSpeedTB(types::MilesPerHour CommandedSpeedTB){
        CommandedSpeedWS_ = CommandedSpeedTB;
    }

    void SoftwareTrainController::SetCurrentVelocityTB(types::MilesPerHour CurrentVelocityTB)
    {
        CurrentVelocity_ = CurrentVelocityTB *0.44704;
    }


    void SoftwareTrainController::CalculateCommandedPower(types::MilesPerHour SetPointSpeed)
    {
        //P(t) = Kp*[V_cmd(t) - v(t)]  +  Ki*∫[Vcmd(τ) - ActualSpeed(τ)]dτ

        // types::BlockId currentBlock = logic to get block
        float blockSpeedLimit = 50.0; // default speed limit atm

        //Defining Vcmd and Actual speed in m/s
        types::MetersPerSecond Vsetpoint = SetPointSpeed*0.44704;

        // find block in hash map
        /*if (blueBlockDataMap.find(currentBlock) != blueBlockDataMap.end()) {
            blockSpeedLimit = blueBlockDataMap[currentBlock][2]; // speed limit in km/hr
        }*/       

        // convert to m/s from km/hr
        blockSpeedLimit *= 0.27778;

        if(Vsetpoint > blockSpeedLimit) {
            Vsetpoint = blockSpeedLimit;
        }

        //Calculating Verror
        float Verror = Vsetpoint - CurrentVelocity_;

        //Calculating Kp term
        float KPterm = Verror*Kp_;

        //Temp time passed since last update
        float deltaTime = 0.5;

        //This section is where the integral section of the equation will be calculated
        IntegralSUM  += Verror * deltaTime; 
        
        //Calculating Ki term
        float KIterm = Ki_*IntegralSUM;

        //Calculating Commanded Power
        types::Watts CommandedPower_ = KPterm + KIterm;

        // Calculate max power
        types::Watts maxPowerLimit = CalculateMaxPower();

        // Clamp CommandedPower_ based on max power
        if (CommandedPower_ > maxPowerLimit) {
            CommandedPower_ = maxPowerLimit;
        } 
        else if (CommandedPower_ < 0) {
            CommandedPower_ = 0; // no negative power
        }
    }

    types::Watts SoftwareTrainController::CalculateMaxPower() {
        // Given constraints for maximum power via datasheet
        const float mass_loaded = 56700; // in kg, fully loaded train
        const float max_acceleration = 0.5; // in m/s^2
        const float max_speed = 70 * 0.27778; // 70km/h to m/s

        // F = m * a
        float force = mass_loaded * max_acceleration;

        // P = F * v
        types::Watts maxPower = force * max_speed;

        return maxPower;
    }

    types::Meters SoftwareTrainController::CalculateDistanceTraveled(float deltaTime) {
        // Distance = speed * time
        types::Meters distanceTraveledThisStep = CurrentVelocity_ * deltaTime;
        types::Meters totalDistanceTraveled;
        //types::BlockId currentBlock = // logic to get block ID

        /*

        // Loop to handle block transistions
        while (distanceTraveledThisStep > 0 && currentBlock <= blueBlockDataMap.size()) {
            float blockLength = blueBlockDataMap[currentBlock][0]; // Block length in meters

            // If distance traveled is greater than current block's length, move to next block
            if (distanceTraveledThisStep >= blockLength) {
                totalDistanceTraveled += blockLength; // add block length to total distance
                distanceTraveledThisStep -= blockLength;
                currentBlock++;
            }
            else {
                // if remaining distance fits within current block, add to total and exit
                totalDistanceTraveled += distanceTraveledThisStep;
                distanceTraveledThisStep = 0;
            }
        }
        return totalDistanceTraveled;

        */
    }
}