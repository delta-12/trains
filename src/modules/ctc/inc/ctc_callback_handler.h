/*****************************************************************************
* @file ctc_callback_handler.h
*
* @brief Implements the CTC UI callbacks.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_CALLBACK_HANDLER_H
#define TRAINS_SRC_MODULES_CTC_INC_CTC_CALLBACK_HANDLER_H

#include "channel.h"
#include "ctc.h"
#include "launcher.h"

namespace ctc
{

void setup_ui(ui::CtcUi &ctc_ui);
void backend_handler(ctc::Ctc &ctc_office);

}

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CALLBACK_HANDLER_H