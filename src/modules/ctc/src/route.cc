#include "route.h"
#include "types.h"
#include <string>

Route::Route(void) {}

Route::Route(
    const types::TrainId &train_id,
    const std::string &train_name,
    std::map<std::string, std::string> destination_and_time
) {
    train_id_ = train_id;
    train_name_ = train_name;
    destination_and_time_ = destination_and_time;
}

types::TrainId Route::getTrainId(void) const {
    return train_id_;
}

std::string Route::getTrainName(void) const {
    return train_name_;
}

std::map<std::string, std::string> Route::getSchedule(void) const {
    return destination_and_time_;
}