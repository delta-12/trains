#include "train_controller_gateway.h"

#include "logger.h"

namespace train_controller
{

types::Error Gateway::UpdateTrainModels(std::vector<std::shared_ptr<train_model::TrainModel>> &trains)
{
    types::Error error = types::ERROR_NONE;

    // TODO NNF-162
    LOGGER_UNUSED(trains); // temporary fix to remove compiler warnings

    return error;
}

} // namespace train_controller