#include "ctc_callback_handler.h"

namespace ctc
{

// Callback for manual dispatch
void handle_manual_dispatch(slint::ComponentHandle<ui::CtcUi> &ctc_ui, ctc::Ctc& ctc, Channel<std::string> &channel)
{
    if (channel.DataAvailable())
    {
        std::string train_id = std::string(ctc_ui->get_train_id());
        std::string arrival_station = std::string(ctc_ui->get_arrival_station());
        std::string destination = channel.Receive();
        if (destination == "Station") {
            types::BlockId destination_block = 0;
            std::vector<ctc::Station> stations = ctc.GetStations();
            for (const ctc::Station &station : stations) {
                if (station.station_name == arrival_station) {
                    destination_block = station.block_id;
                }
            }

            ctc::Train new_train;
            ctc.ManualDispatch(new_train.train_id, destination_block);
            auto train_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
            train_entry->push_back(slint::StandardListViewItem(std::to_string(new_train.train_id).c_str()));
            train_entry->push_back(slint::StandardListViewItem("Yard"));
            train_entry->push_back(slint::StandardListViewItem("_"));
            train_entry->push_back(slint::StandardListViewItem(arrival_station.c_str()));
            auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ctc_ui->get_train_schedules());
            received_train_schedules->push_back(train_entry);

            ctc_ui->set_train_id(std::to_string(new_train.train_id).c_str());
            ctc_ui->set_authority(ctc.GetTrainAuthority(new_train.train_id));
            ctc_ui->set_suggested_speed(static_cast<int>(new_train.suggested_speed));

            auto received_train_ids = std::dynamic_pointer_cast<slint::VectorModel<slint::SharedString>>(ctc_ui->get_trains());
            received_train_ids->push_back(slint::SharedString(std::to_string(new_train.train_id)));
        }
        else {
            if (train_id == "New Train") { 
                ctc::Train new_train;
                ctc.ManualDispatch(new_train.train_id, static_cast<uint16_t>(std::stoi(destination)));
                std::cout << "Train dispatch to: " << destination << std::endl << std::flush;
                auto train_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
                train_entry->push_back(slint::StandardListViewItem(std::to_string(new_train.train_id).c_str()));
                train_entry->push_back(slint::StandardListViewItem("Yard"));
                train_entry->push_back(slint::StandardListViewItem("_"));
                train_entry->push_back(slint::StandardListViewItem(destination.c_str()));
                auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ctc_ui->get_train_schedules());
                received_train_schedules->push_back(train_entry);

                ctc_ui->set_train_id(std::to_string(new_train.train_id).c_str());
                ctc_ui->set_authority(ctc.GetTrainAuthority(new_train.train_id));
                ctc_ui->set_suggested_speed(static_cast<int>(new_train.suggested_speed));

                auto received_train_ids = std::dynamic_pointer_cast<slint::VectorModel<slint::SharedString>>(ctc_ui->get_trains());
                received_train_ids->push_back(slint::SharedString(std::to_string(new_train.train_id)));
            }
            else {
                ctc.ManualDispatch(std::stoi(train_id), static_cast<uint16_t>(std::stoi(destination)));
                auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ctc_ui->get_train_schedules());
                auto train_entry      = std::dynamic_pointer_cast<slint::VectorModel<slint::StandardListViewItem>>(received_train_schedules->row_data(0).value());
                auto destinations = train_entry->row_data(3).value();
                destinations.text = destinations.text + ", " + destination.c_str();
                train_entry->set_row_data(3, destinations);
            }
        }
        // Show Authority and Route
        // ctc::Train                 train;
        // types::Error error = ctc.GetTrainById(1, train);
        // if (error != types::Error::ERROR_NONE) {
        //     std::cout << "Error train id" << std::endl;
        // }
        // std::queue<types::BlockId> authority = train.authority;
        // std::cout << "Authority: " << authority.size() << std::endl;
        // while (!authority.empty())
        // {
        //     std::cout << "Section: " << ctc.GetBlockById(authority.front()).section << " Block: " << authority.front() << std::endl;
        //     authority.pop();
        // }

        // Update UI with train information
        // auto train_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
        // train_entry->push_back(slint::StandardListViewItem(std::to_string(train.train_id).c_str()));
        // train_entry->push_back(slint::StandardListViewItem("Yard"));
        // train_entry->push_back(slint::StandardListViewItem("_"));
        // train_entry->push_back(slint::StandardListViewItem(destination.c_str()));

        // auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ctc_ui->get_train_schedules());
        // received_train_schedules->push_back(train_entry);

        // auto received_train_ids = std::dynamic_pointer_cast<slint::VectorModel<slint::SharedString>>(ctc_ui->get_trains());
        // received_train_ids->push_back(slint::SharedString(std::to_string(train.train_id)));
        // Update other UI elements
        // ctc_ui->set_train_id(std::to_string(train.train_id).c_str());
        // ctc_ui->set_authority(ctc.GetTrainAuthority(train.train_id));
        // ctc_ui->set_suggested_speed(static_cast<int>(train.suggested_speed));

    }
}

void handle_set_occupancy(slint::ComponentHandle<ui::CtcUi> &ctc_ui, ctc::Ctc& ctc, std::shared_ptr<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>> received_train_schedules, Channel<std::string> &channel)
{
    if (channel.DataAvailable())
    {
        std::string train_id = std::string(ctc_ui->get_train_tb());
        types::BlockId occupied_block = static_cast<uint16_t>(std::stoi(channel.Receive()));
        std::cout << "Occupied Block Signal Sent: " << occupied_block << std::endl;
        std::vector<types::BlockState> block_states = { types::BlockState(occupied_block, true, false) };
        ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);

        ctc::Train                 train(0);
        types::Error error = ctc.GetTrainById(std::stoi(train_id), train);
        if (error != types::Error::ERROR_NONE) {
            std::cout << "Error train id" << std::endl;
        }

        std::queue<types::BlockId> authority = train.authority;
        std::cout << "\nAuthority Updated: " << authority.size() << std::endl;
        std::cout << "Train Current Position: " << train.current_position << std::endl;
        while (!authority.empty())
        {
            std::cout << "Block: " << authority.front() << std::endl;
            authority.pop();
        }

        auto train_entry      = std::dynamic_pointer_cast<slint::VectorModel<slint::StandardListViewItem>>(received_train_schedules->row_data(0).value());
        auto current_position = train_entry->row_data(1).value();
        current_position.text = std::to_string(train.current_position);
        auto destination = train_entry->row_data(3).value();
        destination.text = std::to_string(train.destination_list[CTC_TRAIN_CURRENT_DESTINATION].destination);
        train_entry->set_row_data(1, current_position);
        if (destination.text == "0") {
            destination.text = "Yard";
            train_entry->set_row_data(3, destination);
        }
        else {
            train_entry->set_row_data(3, destination);
        }

        // Update Testbench with Train 1 info
        ctc_ui->set_authority(ctc.GetTrainAuthority(std::stoi(train_id)));
        ctc_ui->set_suggested_speed(static_cast<int>(ctc.GetTrainSuggestedSpeed(std::stoi(train_id))));
    }
}
}