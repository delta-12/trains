/*****************************************************************************
 * @file track_model_tests.cc
 *
 * @brief Short-term testing for the Track Model backend.
 *****************************************************************************/

#include "track_model.h"
#include "track_model_ui.h"
#include "csv_parser.h"
#include "types.h"
#include <iostream>
#include <fstream>

#include <filesystem>
#include <string>
#include <sstream>
#include <vector>


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
    auto track_model_ui = ui::TrackModelUi::create();
    std::string input_file_path="";

    track_model_ui->on_choose_file([&]() {
        // For other platforms
        std::string path;
        std::cout << "Please enter the path to the CSV file: ";

        std::getline(std::cin, input_file_path);

        // Check if the file exists
        if (std::filesystem::exists(input_file_path)) {
            std::cout << "Selected file: " << input_file_path << std::endl;
            track_model_ui->set_selected_fileName(ExtractFileName(input_file_path).c_str());
        } else {
            std::cout << "File does not exist." << std::endl;
            track_model_ui->set_selected_fileName("No file selected");
        }

        track_model_ui->on_parse_file([&]() {
            std::cout << "Parsing File..." << std::endl;

            std::filesystem::path path(input_file_path);
            CsvParser parser(path);

            std::vector<std::vector<std::string> > records = parser.GetRecords();
            std::cout << "Track size: " << parser.GetSize() << std::endl;
            for(const std::vector<std::string> &record : records)
            {
                for(const std::string &value : record)
                {
                    std::cout << value << ", ";
                }
                std::cout << "Record Size: " << record.size();
                std::cout << std::endl;
            }

            track_model::SoftwareTrackModel trackModel(records);
        });    
    });
    track_model_ui->run();

    return 0;
}