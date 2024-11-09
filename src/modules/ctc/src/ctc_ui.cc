/*****************************************************************************
 * @file ctc_ui.cc
 *
 * @brief Implement Ctc UI.
 *****************************************************************************/

#include "ctc.h"
#include "ctc_ui.h"

#include <iostream>
#include <vector>

int main(void)
{
    ctc::Ctc ctc;
    auto ctc_ui = ui::CtcUi::create();

    std::filesystem::path base_path = std::filesystem::current_path();
    std::filesystem::path path      = base_path  / "tests" / "common" / "test_csv" / "green_line_schedule.csv";
    ctc.SetScheduleFilePath(path);
    ctc.SetTrackLayout();

    std::cout << "Number of Stations: " << ctc.GetNumStation() << std::endl;
    
    // auto row_data_model = std::make_shared<slint::VectorModel<slint::VectorModel<slint::StandardListViewItem>>>();
    // ctc_ui->set_train_schedules(row_data_model);

    // Populate Destination Station ComboBox
    auto stations_model = std::make_shared<slint::VectorModel<slint::SharedString>>();
    std::vector<ctc::Station> stations = ctc.GetStations();
    for (const ctc::Station &station : stations) {
        stations_model->push_back(station.station_name.c_str());
    }
    ctc_ui->set_stations(stations_model);


    // auto list_model = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
    // list_model->push_back(slint::StandardListViewItem({text: "Train 1"}));
    // list_model->push_back(slint::StandardListViewItem({text: "Train 2"}));
    // ctc_ui->set_list(list_model);

    ctc_ui->on_manual_dispatch([&] {
        int destination = std::stoi(std::string(ctc_ui->get_destination()));
        std::cout << "Destination Block: " << destination << std::endl;
        ctc.ManualDispatch(destination);
        ctc::Train train = ctc.GetTrainById(1);
        while (!train.authority.empty())
        {
            std::cout << "Block: " << train.authority.front() << std::endl;
            train.authority.pop();
        }

        std::string station = std::string(ctc_ui->get_station());
        std::cout << "Selected Station: " << station << std::endl;
    });

    ctc_ui->run();
}