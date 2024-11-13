/*****************************************************************************
* @file ctc_callback_handler.h
*
* @brief Implements the CTC UI callbacks.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_CALLBACK_HANDLER_H
#define TRAINS_SRC_MODULES_CTC_INC_CTC_CALLBACK_HANDLER_H

#include "launcher.h"
#include "ctc.h"

namespace ctc_callback_handler {
    void on_manual_dispatch(slint::ComponentHandle<ui::CtcUi> ctc_ui, ctc::Ctc& ctc);
    void setup_ctc_callbacks(slint::ComponentHandle<ui::CtcUi> ctc_ui, ctc::Ctc& ctc);
}

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CALLBACK_HANDLER_H