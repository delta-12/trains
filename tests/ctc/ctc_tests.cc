/*****************************************************************************
 * @file ctc_tests.cc
 *
 * @brief Short-term testing for the CTC backend.
 *****************************************************************************/

#include "ctc.h"
#include "ctc_ui.h"

int main(void)
{
    auto ctc_ui = ui::CtcUi::create();

    ctc_ui->run();

    return 0;
}