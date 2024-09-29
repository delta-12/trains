/*****************************************************************************
 * @file wayside_controller_tests.cc
 *
 * @brief Short-term testing for the Wayside Controller backend.
 *****************************************************************************/

#include "wayside_controller.h"
#include "wayside_controller_ui.h"

int main(void)
{
    auto wayside_controller_ui = ui::WaysideControllerUi::create();

    wayside_controller_ui->run();

    return 0;
}