/*****************************************************************************
 * @file ctc_tests.cc
 *
 * @brief Short-term testing for the CTC backend.
 *****************************************************************************/

#include "ctc.h"
#include "ctc_ui.h"
#include "windows.h"
#include "commdlg.h"

std::string extractFileName(const std::string& fullPath) {
    // Find the last occurrence of backslash
    size_t pos = fullPath.find_last_of("\\/");
    if (pos != std::string::npos) {
        return fullPath.substr(pos + 1); // Return everything after the last backslash
    } else {
        return fullPath; // If no backslash is found, return the full string (it may already be a file name)
    }
}

int main(void)
{
    auto ctc_ui = ui::CtcUi::create();

    ctc_ui->on_choose_file([&]() {
        OPENFILENAME ofn;       // Common dialog box structure
        char fileName[MAX_PATH]; // Buffer for the file name
        ZeroMemory(&ofn, sizeof(ofn)); // Clear the structure
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = nullptr; // If using in a GUI, set to the parent window handle
        ofn.lpstrFile = fileName;
        ofn.lpstrFile[0] = '\0';  // Initialize buffer to empty string
        ofn.nMaxFile = sizeof(fileName);
        ofn.lpstrFilter = "Excel Files\0*.XLS;*.XLSX\0CSV Files\0*.CSV\0All Files\0*.*\0";
        ofn.nFilterIndex = 1; // Default to the first filter (Excel)
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = nullptr; // Default directory
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        // Open the file dialog
        if (GetOpenFileName(&ofn)) {
            std::cout << "Selected file: " << ofn.lpstrFile << std::endl;
            ctc_ui->set_fileName(extractFileName(ofn.lpstrFile).c_str());
        } else {
            std::cout << "No file selected." << std::endl;
            ctc_ui->set_fileName("No file selected");
        }
    });

    ctc_ui->run();

    return 0;
}