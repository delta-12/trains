/*****************************************************************************
* @file ctc.h
*
* @brief Implements the CTC backend.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_CTC_INC_CTC_CC
#define TRAINS_SRC_MODULES_CTC_INC_CTC_CC

#include "wayside_controller_gateway.h"
#include "ctc.h"
#include "windows.h"
#include "commdlg.h"

#include <sstream>
#include "unordered_map"
#include <iostream>


namespace ctc
{
static std::string ExtractFileName(const std::string& full_path);

/*------------------------------ Ctc Implementation ------------------------------*/
void Ctc::LoadSchedule(const std::string &file_path) {
    // Need Implementation
}

types::Error Ctc::OpenFileExplorer(std::string &file) {
    types::Error error;
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
        std::cout << "File Path: " << ofn.lpstrFile << std::endl;
        schedule_file_path = ofn.lpstrFile;
        file = ExtractFileName(ofn.lpstrFile);
        std::cout << "File Name: " << file << std::endl;
        error = types::Error::ERROR_NONE;
    } else {
        error = types::Error::ERROR_INVALID_FORMAT;
    }
    return error;
}

// void Ctc::ManualDispatch(std::string &arrival_time, std::string &station_name, std::string &train_name, types::TrackId track_id) {
//     auto it = std::find_if(train_schedule_.begin(), train_schedule_.end(),
//                            [&](const auto &pair) {
//                                return pair.second.train_name == train_name;
//                            });
//     ctc::Line line_info = GetLine(track_id);

//     // Prepare DestinationAndArrivalTime object
//     ctc::Station station = line_info.GetStationByStationName(station_name);
//     std::string arrival_t = arrival_time;
//     types::Tick t;
//     DestinationAndArrivalTime dst(station, t);

//     if (it != train_schedule_.end())
//     {
//         it->second.destination_list.push_back(dst);
//     }
//     else
//     {
//         Train new_train(train_name);
//         new_train.destination_list.push_back(dst);
//         // Implement Suggested Speed and Authority
//         train_schedule_.insert({new_train.train_id, new_train});
//     }
// }

ctc::Train Ctc::GetTrainByName(const std::string train_name)
{
}

static std::string ExtractFileName(const std::string& full_path) {
    // Find the last occurrence of backslash
    size_t pos = full_path.find_last_of("\\/");
    if (pos != std::string::npos) {
        return full_path.substr(pos + 1); // Return everything after the last backslash
    } else {
        return full_path; // If no backslash is found, return the full string (it may already be a file name)
    }
}

} // namespace ctc

#endif // TRAINS_SRC_MODULES_CTC_INC_CTC_CC