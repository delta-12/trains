#include "ctc_callback_handler.h"

#include <string>

#include "channel.h"
#include "launcher.h"

namespace ctc
{

// TODO repeat for each event listener
static Channel<std::string> destination_channel;

static Channel<std::string> ctc_block_occupancy_channel;
static Channel<std::string> ctc_manual_dispatch_channel;

static void register_callbacks(ui::CtcUi &ctc_ui);

// TODO repeat for each event listener
static inline void manual_dispatch_callback(ui::CtcUi &ctc_ui);
static void manual_dispath_handler(ctc::Ctc &ctc_office);

void setup_ui(ui::CtcUi &ctc_ui)
{
    register_callbacks(ctc_ui);

    auto block_data_model = std::make_shared<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(); 
    std::vector<types::Block> blocks = ctc.GetBlocks(); 
    blocks.erase(blocks.begin());
    for (const types::Block &block : blocks) {
        auto block_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
        block_entry->push_back(slint::StandardListViewItem({text: std::string(1, block.section).c_str()}));
        block_entry->push_back(slint::StandardListViewItem({text: std::to_string(block.block).c_str()}));
        block_entry->push_back(slint::StandardListViewItem({text: block.maintenance ? "Maintenance" : "Open"}));
        block_entry->push_back(slint::StandardListViewItem({text: "_"}));
        block_entry->push_back(slint::StandardListViewItem({text: block.occupied ? "Occupied" : "_"}));
        block_entry->push_back(slint::StandardListViewItem({text: block.power_failure ? "Failure" : "_"}));
        block_data_model->push_back(block_entry);
    }
    ctc_ui->set_block_data(block_data_model);
    // Create Model to Populate Train Schedule Table
    auto train_schedule_model = std::make_shared<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>();
    ctc_ui->set_train_schedules(train_schedule_model);
    auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ctc_ui->get_train_schedules());
    
    // Create Model to Populate Trains Dropdown
    auto train_ids = std::make_shared<slint::VectorModel<slint::SharedString>>();
    train_ids->push_back(slint::SharedString("New Train"));
    ctc_ui->set_trains(train_ids);
    auto received_train_ids = std::dynamic_pointer_cast<slint::VectorModel<slint::SharedString>>(ctc_ui->get_trains());

    // Create Model to Populate Stations
    auto stations_model = std::make_shared<slint::VectorModel<slint::SharedString>>();
    std::vector<ctc::Station> stations = ctc.GetStations();
    for (const ctc::Station &station : stations) {
        stations_model->push_back(station.station_name.c_str());
    }
    ctc_ui->set_stations(stations_model);
}

// This will be called in the backend each time through the loop
void backend_handler(ctc::Ctc &ctc_office)
{
    manual_dispath_handler(ctc_office);

    // TODO repeat for each callback that needs to be handled in the backend
}

static void register_callbacks(ui::CtcUi &ctc_ui)
{
    ctc_ui->on_manual_dispatch([&ctc_ui] {
        manual_dispatch_callback(ctc_ui);
    });

    // TODO repeat for each event listener
}

// Callbacks run on the frontend thread
static inline void manual_dispatch_callback(ui::CtcUi &ctc_ui)
{
    destination_channel.Send(ctc_ui->get_destination());

    // TODO update UI if necessary
}

// Handlers run on the backend thread
static void manual_dispath_handler(ctc::Ctc &ctc_office)
{
    if (destination_channel.DataAvailable())
    {
        // TODO need a function that instantiates a new train model and train controller with the same id
        ctc_office.ManualDispatch(0, destination_channel.Receive());

        // Update the UI
        // 1. set channels with appropriate data
        // 2. invoke_from_event_loop to read data from channels in frontend thread
    }
}






// TODO refactor anything below here to match the functions defined above
// Callback for manual dispatch
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
void handle_set_block_states(ctc::Ctc& ctc,
                             std::shared_ptr<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>> &received_train_schedules,
                             std::shared_ptr<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>> &block_data_model,
                             Channel<std::vector<types::BlockState>> &channel)
{
    if (channel.DataAvailable())
    {
        std::vector<types::BlockState> block_states = channel.Receive();
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


        // Update Block Data Table
        std::vector<types::Block> blocks = ctc.GetBlocks();
        blocks.erase(blocks.begin());
        for (const types::BlockState &block_state : block_states)
        {
            types::BlockId block_id    = block_state.block;
            auto           block_entry = std::dynamic_pointer_cast<slint::VectorModel<slint::StandardListViewItem>>(block_data_model->row_data(block_id - 1).value());
            block_entry->set_row_data(4, slint::StandardListViewItem("Occupied"));
            block_data_model->push_back(block_entry);
        }
    }
}
}