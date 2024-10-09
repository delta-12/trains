/*****************************************************************************
* @file block.h
*
* @brief Implements a basic block object.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_ROUTE_H
#define TRAINS_SRC_MODULES_CTC_INC_ROUTE_H

#include "types.h"
#include "map"
#include <string>
#include <vector>

struct Route
{
    types::TrainId train_id;
    std::string train_name;
    std::map<std::string, std::string> destination_and_time;
};

#endif // TRAINS_SRC_MODULES_CTC_INC_ROUTE_H