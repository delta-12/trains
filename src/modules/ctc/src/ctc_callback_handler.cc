#include "ctc_callback_handler.h"

namespace ctc
{

// Callback for manual dispatch
void handle_manual_dispatch(slint::ComponentHandle<ui::CtcUi> &ctc_ui, ctc::Ctc& ctc, Channel<std::string> &channel)
{
    if (channel.DataAvailable())
    {
        std::string destination = channel.Receive();
        ctc.ManualDispatch(static_cast<uint16_t>(std::stoi(destination)));
        std::cout << "Train dispatch to: " << destination << std::endl;

        // Show Authority and Route
        ctc::Train                 train     = ctc.GetTrainById(1);
        std::queue<types::BlockId> authority = train.authority;
        std::cout << "Authority: " << authority.size() << std::endl;
        while (!authority.empty())
        {
            std::cout << "Section: " << ctc.GetBlockById(authority.front()).section << " Block: " << authority.front() << std::endl;
            authority.pop();
        }

        // Update UI with train information
        auto train_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
        train_entry->push_back(slint::StandardListViewItem(std::to_string(train.train_id).c_str()));
        train_entry->push_back(slint::StandardListViewItem("Yard"));
        train_entry->push_back(slint::StandardListViewItem("_"));
        train_entry->push_back(slint::StandardListViewItem(destination.c_str()));

        auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ctc_ui->get_train_schedules());
        received_train_schedules->push_back(train_entry);

        // Update other UI elements
        ctc_ui->set_train_id(train.train_id);
        ctc_ui->set_authority(train.authority.size());
        ctc_ui->set_suggested_speed(static_cast<int>(train.suggested_speed));
    }
}

void handle_set_occupancy(slint::ComponentHandle<ui::CtcUi> &ctc_ui, ctc::Ctc& ctc, std::shared_ptr<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>> received_train_schedules, Channel<std::string> &channel)
{
    if (channel.DataAvailable())
    {
        types::BlockId occupied_block = static_cast<uint16_t>(std::stoi(channel.Receive()));
        std::cout << "Occupied Block Signal Sent: " << occupied_block << std::endl;
        std::vector<types::BlockState> block_states = { types::BlockState(occupied_block, true, false) };
        ctc.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);

        ctc::Train                 train     = ctc.GetTrainById(1);
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
        train_entry->set_row_data(1, current_position);

        ctc_ui->set_authority(train.authority.size());
        ctc_ui->set_suggested_speed(static_cast<int>(train.suggested_speed));
    }
}
}