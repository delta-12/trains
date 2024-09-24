/*****************************************************************************
 * @file train_controller_gateway.h
 *
 * @brief Implements communication between the Train Models and Controllers .
 *****************************************************************************/
#ifndef TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_GATEWAY_H
#define TRAINS_SRC_MODULES_TRAIN_CONTROLLER_INC_TRAIN_CONTROLLER_GATEWAY_H

#include <memory>
#include <vector>

#include "train_model.h"
#include "types.h"

namespace train_controller
{

class Gateway
{
public:
    void UpdateControllers(void);
    types::Error UpdateSimulation(std::shared_ptr<train_model::TrainModel> train_model);
    void SetSimulationMode(const bool simulation);
    bool GetSimulationMode(void) const;
    void AttachPort(std::shared_ptr<types::Port> port);
    void GetAvailableTrains(std::vector<types::TrainId> &trains) const;
    void DispatchTrain(const types::TrainId train, const types::LineId line);
};

} // namespace train_controller

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_TRAIN_CONTROLLER_GATEWAY_H