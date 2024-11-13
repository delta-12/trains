#include "ctc_callback_handler.h"

namespace ctc_callback_handler {

    // Callback for manual dispatch
    void on_manual_dispatch(std::shared_ptr<ui::CtcUi> ctc_ui, ctc::Ctc& ctc) {
        std::string destination = std::string(ctc_ui->get_destination());
        ctc.ManualDispatch(static_cast<uint16_t>(std::stoi(destination)));
        std::cout << "Train dispatch to: " << destination << std::endl;

        // Show Authority and Route
        ctc::Train train = ctc.GetTrainById(1);
        std::queue<types::BlockId> authority = train.authority;
        std::cout << "Authority: " << authority.size() << std::endl;
        while (!authority.empty()) {
            std::cout << "Section: " << ctc.GetBlockById(authority.front()).section << " Block: " << authority.front() << std::endl;
            authority.pop();
        }

        // Update UI with train information
        auto train_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
        train_entry->push_back(slint::StandardListViewItem({text: std::to_string(train.train_id).c_str()}));
        train_entry->push_back(slint::StandardListViewItem({text: "Yard"}));
        train_entry->push_back(slint::StandardListViewItem({text: "_"}));
        train_entry->push_back(slint::StandardListViewItem({text: destination.c_str()}));

        auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ctc_ui->get_train_schedules());
        received_train_schedules->push_back(train_entry);

        // Update other UI elements
        ctc_ui->set_train_id(train.train_id);
        ctc_ui->set_authority(train.authority.size());
        ctc_ui->set_suggested_speed(static_cast<int>(train.suggested_speed));
    }
}