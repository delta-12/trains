/*****************************************************************************
 * @file ctc_tests.cc
 *
 * @brief Short-term testing for the CTC backend.
 *****************************************************************************/

#include "ctc.h"
#include "ctc_ui.h"
#include "csv_parser.h"

#ifdef WIN32
#include "windows.h"
#include "commdlg.h"
#endif

int main(void)
{
    auto ctc_ui = ui::CtcUi::create();
    ctc_ui->run();
    return 0;
}