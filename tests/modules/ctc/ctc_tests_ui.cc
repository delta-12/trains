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

        // ctc.ManualDispatch(1);
    });

    ctc_ui->on_manual_dispatch([&]() {
        int block_id = std::stoi(std::string(ctc_ui->get_destination()));
        types::BlockId destination = (uint16_t)block_id;
        std::cout << "Destination: " << destination << std::endl;

        ctc.ManualDispatch(destination);

        ctc::Train train = ctc.GetTrainById(1);
        std::queue authority = train.authority;
        std::cout << "Authority: " << ctc.GetTrainAuthority(1) <<  std::endl;
        while (!authority.empty()) {
            types::BlockId id = authority.front();
            std::cout << "Block: " << id << std::endl;
            authority.pop();
        }
    });

    ctc_ui->run();
    return 0;
}