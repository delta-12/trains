#include "train_controller.h"
#include "train_model.h"

namespace train_controller
{
    types::Watts TrainController::CalculateCommandedPower(uint8_t speed){
        //P(t) = Kp*[V_cmd(t) - v(t)]  +  Ki*∫[Vcmd(τ) - ActualSpeed(τ)]dτ


        //Defining Vcmd and Actual speed in m/s
        types::MetersPerSecond Vsetpoint = speed*1.609344;
        types::MetersPerSecond ActualSpeed = SetActualSpeedTB() * 1.609344;

        if(Vsetpoint > 70000)
        {
            Vsetpoint = 70000;
        }

        //Calculating Verror
        float Verror = Vsetpoint - ActualSpeed;

        //Calculating Kp term
        float KPterm = Verror*Kp;

        //Temp variation
        float deltaTime = 0.5;

        //This section is where the integral section of the equation will be calculated
        float integralSUM = (Vsetpoint-ActualSpeed)*  deltaTime;
        

        //Calculating Ki term
        float KIterm = Ki*integralSUM;

        //Calculating Commanded Power
        types::Watts CommandedPower = KPterm + KIterm;

        //Returning Commanded Power
        return CommandedPower;
    }



    void SetKP(uint16_t){
        
    }


    void setKI(uint16_t){

    }




}