#include "train_controller.h"
#include "train_model.h"

namespace train_controller
{
    types::Watts TrainController::CalculateCommandedPower(uint8_t speed){
        //P(t) = Kp*[V_cmd(t) - v(t)]  +  Ki*∫[Vcmd(τ) - ActualSpeed(τ)]dτ


        //Defining Vcmd and Actual speed in m/s
        types::MetersPerSecond Vcmd = speed*1.609344;
        types::MetersPerSecond ActualSpeed = SetActualSpeedTB() * 1.609344;

        //Calculating Verror
        float Verror = Vcmd - ActualSpeed;

        //Calculating Kp term
        float KPterm = Verror*Kp;

        //This section is where the integral section of the equation will be calculated
        float currentTime = 1;
        float integral = (Vcmd-ActualSpeed)*currentTime;
        

        //Calculating Ki term
        float KIterm = Ki*integral;

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