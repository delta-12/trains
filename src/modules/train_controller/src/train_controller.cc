#include "train_controller.h"
// #include "train_model.h"
#include "types.h"

namespace train_controller {
// Getters
uint16_t SoftwareTrainController::GetServiceBrake() const {
    return SBPercentage_; // Return the service brake percentage
}

uint16_t SoftwareTrainController::GetGrade(void) const { return 0; }

types::MilesPerHour SoftwareTrainController::GetDriverSpeed(void) const {
    return DriverSpeed_; // Return the current driver speed
}

types::Watts SoftwareTrainController::GetCommandedPower() const {
    return CommandedPower_; // Return the commanded power
}

types::Miles SoftwareTrainController::GetDistanceTravelled(void) const {
    return DistanceTravelled_ / 1609.34;
}

bool SoftwareTrainController::GetEmergencyBrake() const {
    return Ebrake; // Return the state of the emergency brake
}

bool SoftwareTrainController::GetHeadLights() const {
    return HLights; // Return the state of the headlights
}

bool SoftwareTrainController::GetInteriorLights() const {
    return ILights; // Return the state of the interior lights
}

bool SoftwareTrainController::GetLeftDoors() const {
    return LDoor; // Return the state of the left doors
}

bool SoftwareTrainController::GetRightDoors() const {
    return RDoor; // Return the state of the right doors
}

bool SoftwareTrainController::GetBrakeFailure(void) const {
    return BFail; // Return the current state of brake failure
}

bool SoftwareTrainController::GetEngineFailure(void) const {
    return EFail; // Return the current state of engine failure
}

bool SoftwareTrainController::GetSignalPickupFailure(void) const {
    return SigPFail; // Return the current state of signal pickup failure
}

types::DegreesFahrenheit
SoftwareTrainController::GetInternalTemperature() const {
    return Temp; // Return the internal temperature
}

types::DegreesFahrenheit
SoftwareTrainController::GetActInternalTemperature() const {
    return ActualTemp_; // Return the internal temperature
}

types::MilesPerHour SoftwareTrainController::GetCommandedSpeedWS() const {
    return CommandedSpeedWS_; // Return the commanded speed
}

types::MilesPerHour SoftwareTrainController::GetCurrentVelocity() const {
    return CurrentVelocity_; // Return the current velocity
}

types::Blocks SoftwareTrainController::GetAuthority() const {
    return Authority_; // Return the authority block
}

// Setters
void SoftwareTrainController::SetCommandedSpeedWS(types::MilesPerHour speed) {
    CommandedSpeedWS_ = speed;
}

void SoftwareTrainController::SetDriverSpeed(types::MilesPerHour speed) {
    DriverSpeed_ = speed; // Set the driver speed to the provided value
}

void SoftwareTrainController::SetCurrentVelocity(types::MilesPerHour velocity) {
    CurrentVelocity_ = velocity;
}

void SoftwareTrainController::SetServiceBrake(uint16_t percentage) {
    SBPercentage_ = percentage;
}

void SoftwareTrainController::setEmergencyBrake(bool state) { 
    Ebrake = state; 
}

void SoftwareTrainController::SetHeadLights(bool state) { 
    HLights = state; 
}

void SoftwareTrainController::SetInteriorLights(bool state) { 
    ILights = state; 
}

void SoftwareTrainController::SetLeftDoors(bool state) { 
    if(DistanceTravelled == )
        LDoor = state; 
}

void SoftwareTrainController::SetRightDoors(bool state) { 
    RDoor = state; 
}

void SoftwareTrainController::SetBrakeFailure(bool state) {
    BFail = state; // Set the state of brake failure
}

void SoftwareTrainController::SetSignalPickupFailure(bool state) {
    SigPFail = state;
}

void SoftwareTrainController::SetEngineFailure(bool state) { 
    EFail = state; 
}

void SoftwareTrainController::SetInternalTemperature(types::DegreesFahrenheit temp) {
    Temp = temp;
}

void SoftwareTrainController::SetActInternalTemperature(types::DegreesFahrenheit temp) {
    ActualTemp_ = temp;
}

void SoftwareTrainController::SetAuthority(types::Blocks authority) {
    Authority_ = authority;
}

void SoftwareTrainController::SetKP(uint16_t kp) { 
    Kp_ = kp; 
}
void SoftwareTrainController::setKI(uint16_t ki) { 
    Ki_ = ki; 
}

void SoftwareTrainController::SetCommandedSpeedTB(types::MilesPerHour CommandedSpeedTB) {
    CommandedSpeedWS_ = CommandedSpeedTB;
}

void SoftwareTrainController::SetCurrentVelocityTB(types::MilesPerHour CurrentVelocityTB) {
    CurrentVelocity_ = CurrentVelocityTB * 0.44704;
}

void SoftwareTrainController::CalculateCommandedPower() {  
  // P(t) = Kp*[V_cmd(t) - v(t)]  +  Ki*∫[Vcmd(τ) - ActualSpeed(τ)]dτ

  // types::BlockId currentBlock = logic to get block
  float blockSpeedLimit = 50.0; // default speed limit atm

  // Defining Vcmd and Actual speed in m/s
  types::MetersPerSecond Vsetpoint = DriverSpeed_ * 0.44704;

  // find block in hash map
  /*if (blueBlockDataMap.find(currentBlock) != blueBlockDataMap.end()) {
      blockSpeedLimit = blueBlockDataMap[currentBlock][2]; // speed limit in
  km/hr
  }*/

  // convert to m/s from km/hr
  blockSpeedLimit *= 0.27778;

  if (Vsetpoint > blockSpeedLimit) {
    Vsetpoint = blockSpeedLimit;
  }

  // Calculating Verror
  float Verror = Vsetpoint - CurrentVelocity_ * 0.44704;

  // Calculating Kp term
  float KPterm = Verror * Kp_;

  // Temp time passed since last update
  float deltaTime = 1;

  // This section is where the integral section of the equation will be calculated
  IntegralSUM += Verror * deltaTime;

  // Calculating Ki term
  float KIterm = Ki_ * IntegralSUM;


  if(Ebrake == 1)
  {
    IntegralSUM = 0;
    CommandedPower_ = 0;
  }
  { 
    //Checking if Current Train Velocity is greater than Setpoint speed
    if (CurrentVelocity_ > DriverSpeed_) {
        IntegralSUM = 0;
        CommandedPower_ = 0;

        float tempSpeedDiff = CurrentVelocity_ - DriverSpeed_;

        if(tempSpeedDiff >0  && tempSpeedDiff <= 4.35)
        {
        SBPercentage_ = 10;
        }
        else if (tempSpeedDiff > 4.35 && tempSpeedDiff <= 8.7) 
        {
            SBPercentage_ = 20;
        }
        else if(tempSpeedDiff >8.7  && tempSpeedDiff <= 13.05)
        {
            SBPercentage_ = 30;
        }
        else if (tempSpeedDiff > 13.05 && tempSpeedDiff <= 17.4) 
        {
            SBPercentage_ = 40;
        }
        else if (tempSpeedDiff > 17.4 && tempSpeedDiff <= 21.75) 
        {
            SBPercentage_ = 50;
        }
        else if (tempSpeedDiff > 21.75 && tempSpeedDiff <= 26.1) 
        {
            SBPercentage_ = 60;
        }
        else if (tempSpeedDiff > 26.1 && tempSpeedDiff <= 30.45) 
        {
            SBPercentage_ = 70;
        }
        else if (tempSpeedDiff > 30.45 && tempSpeedDiff <= 34.8) 
        {
            SBPercentage_ = 80;
        }
        else if (tempSpeedDiff > 34.8 && tempSpeedDiff <= 39.15) 
        {
            SBPercentage_ = 90;
        }
        else if(tempSpeedDiff > 39.15)
        {
        SBPercentage_ = 100;
        }
    }
    //Checking if Service brake is on
    else if(SBPercentage_ > 0)
    {
        IntegralSUM = 0;
        CommandedPower_ = 0;
    }
    //Normal power calculation
    else {

        CommandedPower_ = KPterm + KIterm;

        if (CommandedPower_ > maxPower) 
        {
        CommandedPower_ = maxPower;
        }
    }


  }

  // Calculating Commanded Power
  // types::Watts CommandedPower_ = KPterm + KIterm;

  // // Clamp CommandedPower_ based on max power
  // if (CommandedPower_ > maxPower) {
  //   CommandedPower_ = maxPower;
  // }
  // else if (CommandedPower_ < 0) {
  //   CommandedPower_ = 0; // no negative power
  // }
}

void SoftwareTrainController::UpdateDistanceTravelled(long interval) {
  DistanceTravelled_ += CurrentVelocity_ * interval;
  
}


// When this is called, the idea is that a second as passed
void SoftwareTrainController::Update()
{
    UpdateDistanceTravelled(1);
    CalculateCommandedPower();



    






}
/*
types::Meters
SoftwareTrainController::CalculateDistanceTraveled(float deltaTime) {
  // Distance = speed * time
  types::Meters distanceTraveledThisStep = CurrentVelocity_ * deltaTime;
  types::Meters totalDistanceTraveled;
  // types::BlockId currentBlock = // logic to get block ID

  

  // Loop to handle block transistions
  while (distanceTraveledThisStep > 0 && currentBlock <=
  blueBlockDataMap.size()) { float blockLength =
  blueBlockDataMap[currentBlock][0]; // Block length in meters

      // If distance traveled is greater than current block's length, move to
  next block if (distanceTraveledThisStep >= blockLength) {
          totalDistanceTraveled += blockLength; // add block length to total
  distance distanceTraveledThisStep -= blockLength; currentBlock++;
      }
      else {
          // if remaining distance fits within current block, add to total and
  exit totalDistanceTraveled += distanceTraveledThisStep;
          distanceTraveledThisStep = 0;
      }
  }
  return totalDistanceTraveled;

  
}*/
} // namespace train_controller