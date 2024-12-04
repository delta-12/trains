#include "ctc_callback_handler.h"

#include <string>

#include "channel.h"
#include "launcher.h"

namespace ctc
{

// TODO repeat for each event listener
static Channel<std::string> destination_channel;


// Train Schedules Channel
static Channel<std::string> train_id_channel;
static Channel<std::string> current_position_channel;
static Channel<std::string> authority_channel;
static Channel<std::string> suggested_speed_channel;


static Channel<std::string> ctc_block_occupancy_channel;
static Channel<std::string> ctc_manual_dispatch_channel;


// Register all Slint callbacks to retrieve input from UI and send to appropriate channel
static void register_callbacks(slint::ComponentHandle<ui::CtcUi> &ctc_ui);

// TODO repeat for each event listener

// Dispatch Specifics
static inline void manual_dispatch_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static void manual_dispatch_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui);

void setup_ui(slint::ComponentHandle<ui::CtcUi> &ctc_ui, ctc::Ctc &ctc)
{
    register_callbacks(ctc_ui);

    auto                      block_data_model = std::make_shared<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>();
    std::vector<types::Block> blocks           = ctc.GetBlocks();
    blocks.erase(blocks.begin());
    for (const types::Block &block : blocks)
    {
        auto block_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
        block_entry->push_back(slint::StandardListViewItem(std::string(1, block.section).c_str()));
        block_entry->push_back(slint::StandardListViewItem(std::to_string(block.block).c_str()));
        block_entry->push_back(slint::StandardListViewItem(block.maintenance ? "Maintenance" : "Open"));
        block_entry->push_back(slint::StandardListViewItem("_"));
        block_entry->push_back(slint::StandardListViewItem(block.occupied ? "Occupied" : "_"));
        block_entry->push_back(slint::StandardListViewItem(block.power_failure ? "Failure" : "_"));
        block_data_model->push_back(block_entry);
    }
    ctc_ui->set_block_data(block_data_model);
    // Create Model to Populate Train Schedule Table
    auto train_schedule_model = std::make_shared<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>();
    ctc_ui->set_train_schedules(train_schedule_model);

    // Create Model to Populate Trains Dropdown
    auto train_ids = std::make_shared<slint::VectorModel<slint::SharedString>>();
    train_ids->push_back(slint::SharedString("New Train"));
    ctc_ui->set_trains(train_ids);

    // Create Model to Populate Stations
    auto                      stations_model = std::make_shared<slint::VectorModel<slint::SharedString>>();
    std::vector<ctc::Station> stations       = ctc.GetStations();
    for (const ctc::Station &station : stations)
    {
        stations_model->push_back(station.station_name.c_str());
    }
    ctc_ui->set_stations(stations_model);
}

// This will be called in the backend each time through the loop
void backend_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    manual_dispatch_handler(ctc_office, ctc_ui);
    // TODO repeat for each callback that needs to be handled in the backend
}

static void register_callbacks(slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    ctc_ui->on_manual_dispatch([&ctc_ui] {
            manual_dispatch_callback(ctc_ui);
        });

    // TODO repeat for each event listener
}

// Callbacks run on the frontend thread
static inline void manual_dispatch_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    destination_channel.Send(std::string(ctc_ui->get_destination()));
    train_id_channel.Send(std::string(ctc_ui->get_train_id()));

    // TODO update UI if necessary
}

// Handlers run on the backend thread
static void manual_dispatch_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    if (destination_channel.DataAvailable())
    {
        // TODO need a function that instantiates a new train model and train controller with the same id
        std::string train_id    = train_id_channel.Receive();
        std::string destination = destination_channel.Receive();
        std::string current_position;
        std::string authority;
        std::string suggested_speed;

        // Dispatch new train
        if (train_id == "New Train")
        {
            // Backend Dispatch
            ctc::Train new_train;
            ctc_office.ManualDispatch(new_train.train_id, static_cast<uint16_t>(std::stoi(destination)));

            //Capture Variable to update
            current_position = std::to_string(ctc_office.GetTrainCurrentPosition(new_train.train_id));
            train_id         = std::to_string(new_train.train_id);
            authority        = std::to_string(ctc_office.GetTrainAuthority(new_train.train_id));
            suggested_speed  = std::to_string(static_cast<int>(ctc_office.GetTrainSuggestedSpeed(new_train.train_id)));

            // Update UI
            slint::ComponentWeakHandle<ui::CtcUi> weak_ui_handle(ctc_ui);
            slint::invoke_from_event_loop([weak_ui_handle, train_id, current_position, authority, suggested_speed, destination] () {
                    if (auto ui = weak_ui_handle.lock())
                    {
                        if (ui.has_value())
                        {
                            // Push new train entry row to TableView component
                            auto train_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
                            train_entry->push_back(slint::StandardListViewItem(train_id.c_str()));
                            if (current_position == "0")
                            {
                                train_entry->push_back(slint::StandardListViewItem("Yard"));
                            }
                            else
                            {
                                train_entry->push_back(slint::StandardListViewItem(current_position.c_str()));
                            }
                            train_entry->push_back(slint::StandardListViewItem(authority.c_str()));
                            train_entry->push_back(slint::StandardListViewItem(suggested_speed.c_str()));
                            train_entry->push_back(slint::StandardListViewItem(destination.c_str()));
                            auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ui.value()->get_train_schedules());
                            received_train_schedules->push_back(train_entry);

                            // Update Train drop down list
                            auto ui_train_ids = std::dynamic_pointer_cast<slint::VectorModel<slint::SharedString>>(ui.value()->get_trains());
                            ui_train_ids->push_back(slint::SharedString(train_id));
                        }
                    }
                });
        }
        // Add another destination to existing train
        else
        {
            // Backend Dispatch
            ctc_office.ManualDispatch(std::stoi(train_id), static_cast<uint16_t>(std::stoi(destination)));
            // Update UI
            slint::ComponentWeakHandle<ui::CtcUi> weak_ui_handle(ctc_ui);
            slint::invoke_from_event_loop([weak_ui_handle, train_id, destination] () {
                    if (auto ui = weak_ui_handle.lock())
                    {
                        if (ui.has_value())
                        {
                            // Add next destination to train destination column in TrainSchedule Table UI
                            auto train_schedules_ui = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ui.value()->get_train_schedules());
                            auto train_entry        = std::dynamic_pointer_cast<slint::VectorModel<slint::StandardListViewItem>>(train_schedules_ui->row_data(std::stoi(train_id) - 1).value());
                            auto train_destination  = train_entry->row_data(4).value();
                            train_destination.text  = train_destination.text + ", " + destination.c_str();
                            train_entry->set_row_data(4, train_destination);
                        }
                    }
                });
        }
    }
}

}