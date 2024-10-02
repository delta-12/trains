/*****************************************************************************
 * @file wayside_controller_tests.cc
 *
 * @brief Short-term testing for the Wayside Controller backend.
 *****************************************************************************/

#include "tick_source.h"
#include "wayside_controller.h"
#include "wayside_controller_ui.h"

int main(void)
{
    auto wayside_controller_ui = ui::WaysideControllerUi::create();
    TickSource tick_source("07:00:00");

    wayside_controller_ui->on_get_time([&]{
        wayside_controller_ui->set_time(tick_source.GetTimeString().c_str());
    });

    tick_source.Start();
    wayside_controller_ui->run();

    return 0;
}