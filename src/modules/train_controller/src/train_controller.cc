#include "train_controller.h"
#include "train_model.h"

namespace train_controller
{
    types::Watts SoftwareTrainController::GetCommandedPower(void) const{

        //Returning Commanded Power
        return CommandedPower_;
    }



    void SoftwareTrainController::SetKP(uint16_t){
        
    }


    void SoftwareTrainController::setKI(uint16_t){

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

        //Defining Vcmd and Actual speed in m/s
        types::MetersPerSecond Vsetpoint = SetPointSpeed*0.44704;

        if(Vsetpoint > 70000)
        {
            Vsetpoint = 70000;
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

    }
}