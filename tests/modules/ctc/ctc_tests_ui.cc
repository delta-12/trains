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

std::string ExtractFileName(const std::string& full_path) {
    // Find the last occurrence of backslash
    size_t pos = full_path.find_last_of("\\/");
    if (pos != std::string::npos) {
        return full_path.substr(pos + 1); // Return everything after the last backslash
    } else {
        return full_path; // If no backslash is found, return the full string (it may already be a file name)
    }
}

int main(void)
{
    ctc::Ctc ctc;
    auto ctc_ui = ui::CtcUi::create();
    std::string input_file_path = "";

    ctc_ui->on_choose_file([&]() {
#ifdef WIN32
        OPENFILENAME ofn;       // Common dialog box structure
        char file_name[MAX_PATH]; // Buffer for the file name
        ZeroMemory(&ofn, sizeof(ofn)); // Clear the structure
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = nullptr; // If using in a GUI, set to the parent window handle
        ofn.lpstrFile = file_name;
        ofn.lpstrFile[0] = '\0';  // Initialize buffer to empty string
        ofn.nMaxFile = sizeof(file_name);
        ofn.lpstrFilter = "CSV Files\0*.CSV\0";
        ofn.nFilterIndex = 1; // Default to the first filter (Excel)
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = nullptr; // Default directory
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        // Open the file dialog
        if (GetOpenFileName(&ofn)) {
            std::cout << "Selected file: " << ofn.lpstrFile << std::endl;
            input_file_path = ofn.lpstrFile;
            ctc_ui->set_selected_fileName(ExtractFileName(ofn.lpstrFile).c_str());
            std::cout << "Input File Path: " + input_file_path << std::endl;
        } else {
            std::cout << "No file selected." << std::endl;
            ctc_ui->set_selected_fileName("No file selected");
        }
#else
            std::cout << "Operation not supported" << std::endl;
#endif
    });

    ctc_ui->on_on_test_clicked([&]() {
        std::cout << "Button Pressed" << std::endl;
    });

    ctc_ui->on_parse_file([&]() {
        std::cout << "Parsing File..." << std::endl;

        std::filesystem::path path(input_file_path);
        CsvParser parser(path);

        std::vector<std::vector<std::string> > records = parser.GetRecords();
        std::cout << "Schedule size: " << parser.GetSize() << std::endl;
        ctc.PopulateDataFromCsv(records);
        auto it = ctc.train_schedule.begin();
        ctc_ui->set_train_1(it->first.c_str());
        ctc_ui->set_train_1_at(it->second.begin()->second.c_str());
        ctc.PrintAllTrainSchedule();
        ctc.PrintAllBlockMap();
    });

    ctc_ui->on_manual_dispatch([&]() {
        std::string train_name = std::string(ctc_ui->get_train_name());
        std::string destination = std::string(ctc_ui->get_destination());
        std::string arrival_time = std::string(ctc_ui->get_arrival_time());
        ctc.AddTrainScheduleManual(train_name, destination, arrival_time);
        ctc_ui->set_train_1(train_name.c_str());
        ctc_ui->set_train_1_at(arrival_time.c_str());
        ctc_ui->set_train_1_dst(destination.c_str());
    });

    ctc_ui->run();

    return 0;
}