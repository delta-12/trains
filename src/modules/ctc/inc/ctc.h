/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_H
#define TRAINS_SRC_MODULES_CTC_INC_CTC_H

#include "types.h"
#include "wayside_controller_gateway.h"

namespace ctc
{

class Ctc
{
    public:
        types::Error UpdateWaysideControllers(wayside_controller::Gateway &gateway);
};

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_H