/*****************************************************************************
 * @file ctc_tests.cc
 *
 * @brief Short-term testing for the CTC backend.
 *****************************************************************************/

#include "ctc.h"
#include "ctc_ui.h"

#ifdef WIN32
#include "windows.h"
#include "commdlg.h"
#endif

int main(void)
{
    auto ctc_ui = ui::CtcUi::create();

    ctc::Ctc ctc;
    ctc_ui->on_choose_file([&]() {
        std::string file;
        types::Error error = ctc.OpenFileExplorer(file);
        if (error == types::Error::ERROR_NONE) {
            ctc_ui->set_selected_fileName(file.c_str());
        }
        else {
            std::string message = "*Invalid File";
            ctc_ui->set_selected_fileName(message.c_str());
        }

        ctc.SetTrackLayout();
    });

    ctc_ui->on_manual_dispatch([&]() {
        int block_id = std::stoi(std::string(ctc_ui->get_destination()));
        types::BlockId destination = (uint16_t)block_id;

        ctc.ManualDispatch(destination);
    });

    ctc_ui->run();
    return 0;
}