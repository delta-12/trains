/*****************************************************************************
* @file ctc_callback_handler.h
*
* @brief Implements the CTC UI callbacks.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_CALLBACK_HANDLER_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_CALLBACK_HANDLER_H

#include "launcher.h"
//#include "slint.h"
#include "wayside_controller.h"
#include "ctc.h"
#include "channel.h"

namespace wayside_controller
{

void selectWCType(slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui, Channel<std::string> &channel);
void selectLineType(slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui, Channel<std::string> &channel);
void select_file(slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui, Channel<std::string> &channel);
void parse_file(slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui, Channel<std::string> &channel);


//function to refill the data
void load_block_data(slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui, Channel<std::string> &channel);


void fillBlockData(slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui, wayside_controller::WaysideController& wc, wayside_controller::Plc& plc, 
        std::shared_ptr<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>> received_block_data, Channel<std::string> &channel);

//tb callbacks
void disp_sugg_auth(slint::ComponentHandle<ui::WaysideControllerUi> &wayside_controller_ui, wayside_controller::WaysideController &wc, Channel<std::string> &channel);





}

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_CALLBACK_HANDLER_H