/*****************************************************************************
* @file ctc_callback_handler.h
*
* @brief Implements the CTC UI callbacks.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_CALLBACK_HANDLER_H
#define TRAINS_SRC_MODULES_CTC_INC_CTC_CALLBACK_HANDLER_H

#include "launcher.h"
#include "ctc.h"
#include "channel.h"

namespace ctc
{
void handle_manual_dispatch(slint::ComponentHandle<ui::CtcUi> &ctc_ui, ctc::Ctc& ctc, Channel<std::string> &channel);
void handle_set_occupancy(slint::ComponentHandle<ui::CtcUi> &ctc_ui, ctc::Ctc& ctc, std::shared_ptr<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>> received_train_schedules, Channel<std::string> &channel);
}

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CALLBACK_HANDLER_H