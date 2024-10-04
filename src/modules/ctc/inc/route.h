/*****************************************************************************
* @file block.h
*
* @brief Implements a basic block object.
*****************************************************************************/
#ifndef TRAINS_TESTS_CTC_MODEL_ROUTE_H
#define TRAINS_TESTS_CTC_MODEL_ROUTE_H

#include "types.h"
#include "map"
#include <string>
#include <vector>

class Route
{
    private:
        types::TrainId train_id_;
        std::string train_name_;
        std::map<std::string, std::string> destination_and_time_;

    public:
        Route();
        Route(
            const types::TrainId &train_id,
            const std::string &train_name,
            std::map<std::string, std::string> destination_and_time
            );

        types::TrainId getTrainId(void) const;
        std::string getTrainName(void) const;
        std::map<std::string, std::string> getSchedule(void) const;
};

#endif // TRAINS_TESTS_CTC_MODEL_ROUTE_H