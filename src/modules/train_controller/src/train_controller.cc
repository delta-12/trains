#include "train_controller.h"
#include "train_model.h"

namespace train_controller
{
    types::Watts TrainController::CalculateCommandedPower(uint8_t speed){

        //Defining Vcmd and Actual speed in m/s
        types::MetersPerSecond Vcmd = speed*1.609344;
        types::MetersPerSecond ActualSpeed = SetActualSpeedTB() * 1.609344;

        //Calculating Verror
        float Verror = Vcmd - ActualSpeed;

        //Calculating Kp term
        float firstTerm = Verror*Kp;


        //This section is where the integral section of the equation will be calculated
        //P(t) = Kp*[V_cmd(t) - v(t)]  +  Ki*∫[Vcmd(τ) - ActualSpeed(τ)]dτ

        // for (size_t i = 0; i < count; i++)
        // {
        //     /* code */
        // }
        
        


        return types::Watts{/**/};
    }



    void SetKP(uint16_t){
        
    }


    void setKI(uint16_t){

    }




}