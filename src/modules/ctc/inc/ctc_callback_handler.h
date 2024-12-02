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
extern Channel<std::vector<types::BlockState>> wc_ctc_block_occupancy_channel;

void setup_ui(slint::ComponentHandle<ui::CtcUi> &ctc_ui);
void backend_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui);

}

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CALLBACK_HANDLER_H