#include "ctc_callback_handler.h"

namespace ctc
{

// cppcheck-suppress unusedFunction
void handle_manual_dispatch(slint::ComponentHandle<ui::CtcUi> &ctc_ui, ctc::Ctc& ctc, Channel<std::string> &channel)
{
    if (channel.DataAvailable())
    {
        std::string train_id        = std::string(ctc_ui->get_train_id());
        std::string arrival_station = std::string(ctc_ui->get_arrival_station());
        std::string destination     = channel.Receive();
        if (destination == "Station")
        {
            types::BlockId            destination_block = 0;
            std::vector<ctc::Station> stations          = ctc.GetStations();
            for (const ctc::Station &station : stations)
            {
                if (station.station_name == arrival_station)
                {
                    destination_block = station.block_id;
                }
            }

            ctc::Train new_train;
            ctc.ManualDispatch(new_train.train_id, destination_block);
            auto train_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
            train_entry->push_back(slint::StandardListViewItem(std::to_string(new_train.train_id).c_str()));
            train_entry->push_back(slint::StandardListViewItem("Yard"));
            train_entry->push_back(slint::StandardListViewItem(std::to_string(ctc.GetTrainAuthority(new_train.train_id)).c_str()));
            train_entry->push_back(slint::StandardListViewItem(std::to_string(static_cast<int>(ctc.GetTrainSuggestedSpeed(new_train.train_id))).c_str()));
            train_entry->push_back(slint::StandardListViewItem(arrival_station.c_str()));
            auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ctc_ui->get_train_schedules());
            received_train_schedules->push_back(train_entry);

            ctc_ui->set_authority(ctc.GetTrainAuthority(new_train.train_id));
            ctc_ui->set_suggested_speed(static_cast<int>(new_train.suggested_speed));

            auto received_train_ids = std::dynamic_pointer_cast<slint::VectorModel<slint::SharedString>>(ctc_ui->get_trains());
            received_train_ids->push_back(slint::SharedString(std::to_string(new_train.train_id)));
        }
        else if (destination == "All")
        {
            std::vector<types::BlockId> blocks = ctc.GetDefaultRoute();
            for ( const types::BlockId block : blocks )
            {
                ctc::Train new_train;
                ctc.ManualDispatch(new_train.train_id, block);
                auto train_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
                train_entry->push_back(slint::StandardListViewItem(std::to_string(new_train.train_id).c_str()));
                train_entry->push_back(slint::StandardListViewItem("Yard"));
                train_entry->push_back(slint::StandardListViewItem(std::to_string(ctc.GetTrainAuthority(new_train.train_id)).c_str()));
                train_entry->push_back(slint::StandardListViewItem(std::to_string(static_cast<int>(ctc.GetTrainSuggestedSpeed(new_train.train_id))).c_str()));
                train_entry->push_back(slint::StandardListViewItem(std::to_string(block).c_str()));
                auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ctc_ui->get_train_schedules());
                received_train_schedules->push_back(train_entry);

                ctc_ui->set_authority(ctc.GetTrainAuthority(new_train.train_id));
                ctc_ui->set_suggested_speed(static_cast<int>(new_train.suggested_speed));
            }
        }
        else
        {
            if (train_id == "New Train")
            {
                ctc::Train new_train;
                ctc.ManualDispatch(new_train.train_id, static_cast<uint16_t>(std::stoi(destination)));
                std::cout << "Train dispatch to: " << destination << std::endl << std::flush;
                auto train_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
                train_entry->push_back(slint::StandardListViewItem(std::to_string(new_train.train_id).c_str()));
                train_entry->push_back(slint::StandardListViewItem("Yard"));
                train_entry->push_back(slint::StandardListViewItem(std::to_string(ctc.GetTrainAuthority(new_train.train_id)).c_str()));
                train_entry->push_back(slint::StandardListViewItem(std::to_string(static_cast<int>(ctc.GetTrainSuggestedSpeed(new_train.train_id))).c_str()));
                train_entry->push_back(slint::StandardListViewItem(destination.c_str()));
                auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ctc_ui->get_train_schedules());
                received_train_schedules->push_back(train_entry);


                ctc_ui->set_authority(ctc.GetTrainAuthority(new_train.train_id));
                ctc_ui->set_suggested_speed(static_cast<int>(new_train.suggested_speed));

                auto received_train_ids = std::dynamic_pointer_cast<slint::VectorModel<slint::SharedString>>(ctc_ui->get_trains());
                received_train_ids->push_back(slint::SharedString(std::to_string(new_train.train_id)));

            }
            else
            {
                ctc.ManualDispatch(std::stoi(train_id), static_cast<uint16_t>(std::stoi(destination)));
                auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ctc_ui->get_train_schedules());
                auto train_entry              = std::dynamic_pointer_cast<slint::VectorModel<slint::StandardListViewItem>>(received_train_schedules->row_data(std::stoi(train_id) - 1).value());
                auto destinations             = train_entry->row_data(4).value();
                destinations.text = destinations.text + ", " + destination.c_str();
                train_entry->set_row_data(4, destinations);
            }
        }

    }
}

// cppcheck-suppress unusedFunction
void handle_set_occupancy(slint::ComponentHandle<ui::CtcUi> &ctc_ui, ctc::Ctc& ctc,
                          std::shared_ptr<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>> &received_train_schedules,
                          std::shared_ptr<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>> &block_data_model,
                          Channel<std::string> &channel)
{
    if (channel.DataAvailable())
    {
        types::BlockId occupied_block = static_cast<uint16_t>(std::stoi(channel.Receive()));
        std::cout << "Occupied Block Signal Sent: " << occupied_block << std::endl;
        std::vector<types::BlockState> block_states = { types::BlockState(occupied_block, true, false) };
        ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);

        std::vector<ctc::Train> trains = ctc.GetTrains();
        received_train_schedules->clear();
        for (const ctc::Train &train : trains)
        {
            auto train_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
            train_entry->push_back(slint::StandardListViewItem(std::to_string(train.train_id).c_str()));
            train_entry->push_back(slint::StandardListViewItem(std::to_string(train.current_position).c_str()));
            train_entry->push_back(slint::StandardListViewItem(std::to_string(ctc.GetTrainAuthority(train.train_id)).c_str()));
            train_entry->push_back(slint::StandardListViewItem(std::to_string(static_cast<int>(ctc.GetTrainSuggestedSpeed(train.train_id))).c_str()));
            train_entry->push_back(slint::StandardListViewItem(std::to_string(train.destination_list[CTC_TRAIN_CURRENT_DESTINATION].destination).c_str()));
            received_train_schedules->push_back(train_entry);
        }


        // Update Testbench with Train 1 info
        ctc_ui->set_authority(ctc.GetTrainAuthority(1));
        ctc_ui->set_suggested_speed(static_cast<int>(ctc.GetTrainSuggestedSpeed(1)));

        // Update Block Data Table
        auto block_entry = std::dynamic_pointer_cast<slint::VectorModel<slint::StandardListViewItem>>(block_data_model->row_data(occupied_block - 1).value());
        block_entry->set_row_data(4, slint::StandardListViewItem("Occupied"));

    }
}
}