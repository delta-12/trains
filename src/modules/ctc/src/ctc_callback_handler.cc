#include "ctc_callback_handler.h"

#include <string>
#include <sstream>
#include <iostream>

#include "channel.h"
#include "launcher.h"

namespace ctc
{

/*----------------------------------- Channels Start -----------------------------------*/
// Manual Dispatch Channel
static Channel<std::string> destination_channel;
static Channel<std::string> arrival_time_channel;

// Train Schedules Channel
static Channel<std::string> train_id_channel;
static Channel<std::string> current_position_channel;
static Channel<std::string> authority_channel;
static Channel<std::string> suggested_speed_channel;

// Occupancy Related Channels
static Channel<std::vector<types::BlockState>> block_occupancy_channel;
Channel<std::vector<types::BlockState>>        wc_ctc_block_occupancy_channel;

// Maintenance Mode Channels
static Channel<std::vector<types::BlockId>> block_maintenance_channel;
static Channel<std::vector<types::BlockId>> block_fix_channel;
static Channel<std::vector<types::BlockId>> block_input_channel;

// Choose File Channel
static Channel<bool> choose_file_channel;

// Simulation Speed Channel
static Channel<int> simulation_speed_channel;

// Switch Position Channel
static Channel<std::vector<types::BlockId>> switch_block_channel;
static Channel<bool>                        switch_position_channel;

// Automatic Dispatch Channel
static Channel<bool> automatic_dispatch_channel;

// Testbench
static Channel<std::string> train_id_tb_channel;

// Helper
static std::vector<types::BlockId> TokenizeOccupancyInput(const std::string& input, char delimiter);
static void UpdateBlockOccupancyUI(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui, std::vector<types::BlockState> &block_states);
static void UpdateBlockMaintenanceUI(slint::ComponentHandle<ui::CtcUi> &ctc_ui, std::vector<types::Block> &blocks);

// Register all Slint callbacks to retrieve input from UI and send to appropriate channel
static void register_callbacks(slint::ComponentHandle<ui::CtcUi> &ctc_ui);

/*----------------------------------- Callbacks and Hanlders -----------------------------------*/
// Callbacks
static inline void manual_dispatch_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static inline void send_occupancy_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static inline void send_failure_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static inline void set_maintenance_mode_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static inline void fix_block_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static inline void choose_file_callback();
static inline void simulation_speed_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static inline void set_switch_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static inline void automatic_dispatch_callback();
static inline void train_tb_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui);

// Backend Handlers
static void manual_dispatch_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static void send_occupancy_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static void maintenance_mode_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static void fix_block_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui);

static void choose_file_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static void tick_source_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static void throughput_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static void simulation_speed_handler(ctc::Ctc &ctc_office);
static void set_switch_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static void departure_time_handler(ctc::Ctc &ctc_office);
static void automatic_dispatch_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui);
static void train_tb_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui);

void setup_ui(slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    register_callbacks(ctc_ui);

    auto block_data_model = std::make_shared<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>();
    ctc_ui->set_block_data(block_data_model);
    // Create Model to Populate Train Schedule Table
    auto train_schedule_model = std::make_shared<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>();
    ctc_ui->set_train_schedules(train_schedule_model);

    // Create Model to Populate Trains Dropdown
    auto train_ids = std::make_shared<slint::VectorModel<slint::SharedString>>();
    train_ids->push_back(slint::SharedString("New Train"));
    ctc_ui->set_trains(train_ids);

    // Create Model to Populate Trains Dropdown
    auto train_tb_ids = std::make_shared<slint::VectorModel<slint::SharedString>>();
    ctc_ui->set_trains_tb(train_tb_ids);

    // Create Model to Populate Stations
    auto stations_model = std::make_shared<slint::VectorModel<slint::SharedString>>();
    ctc_ui->set_stations(stations_model);

    ctc_ui->set_throughput(std::to_string(0).c_str());
}

// This will be called in the backend each time through the loop
void backend_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    manual_dispatch_handler(ctc_office, ctc_ui);
    send_occupancy_handler(ctc_office, ctc_ui);
    maintenance_mode_handler(ctc_office, ctc_ui);
    fix_block_handler(ctc_office, ctc_ui);
    choose_file_handler(ctc_office, ctc_ui);
    tick_source_handler(ctc_office, ctc_ui);
    throughput_handler(ctc_office, ctc_ui);
    simulation_speed_handler(ctc_office);
    set_switch_handler(ctc_office, ctc_ui);
    departure_time_handler(ctc_office);
    automatic_dispatch_handler(ctc_office, ctc_ui);
    train_tb_handler(ctc_office, ctc_ui);
    // TODO repeat for each callback that needs to be handled in the backend
}

static void register_callbacks(slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    ctc_ui->on_manual_dispatch([&ctc_ui] {
            manual_dispatch_callback(ctc_ui);
        });

    // On clicking Send Block Occupancy
    ctc_ui->on_send_occupancy([&ctc_ui] {
            send_occupancy_callback(ctc_ui);
        });

    // On clicking Send Failure Occupancy
    ctc_ui->on_send_failure_signal([&ctc_ui] {
            send_failure_callback(ctc_ui);
        });

    // On switching Maintenance mode ON
    ctc_ui->on_set_maintenance_mode([&ctc_ui] {
            set_maintenance_mode_callback(ctc_ui);
        });

    // On switching Maintenance mode OFF
    ctc_ui->on_fix_block([&ctc_ui] {
            fix_block_callback(ctc_ui);
        });

    ctc_ui->on_choose_file([&ctc_ui] {
            choose_file_callback();
        });

    ctc_ui->on_set_simulation_speed([&ctc_ui] {
            simulation_speed_callback(ctc_ui);
        });

    ctc_ui->on_set_switch_position([&ctc_ui] {
            set_switch_callback(ctc_ui);
        });

    ctc_ui->on_automatic_dispatch([&ctc_ui] {
            automatic_dispatch_callback();
        });

    // Test Bench
    ctc_ui->on_train_tb_output([&ctc_ui] {
            train_tb_callback(ctc_ui);
        });

    // TODO repeat for each event listener
}

/*----------------------------------- Manual Dispatch -----------------------------------*/
// Callbacks run on the frontend thread
static inline void manual_dispatch_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    if (std::string(ctc_ui->get_arrival_station()).empty())
    {
        ctc_ui->set_dispatch_success(false);
        ctc_ui->set_manual_dispatch_message("Station Input Missing");
    }
    else if (std::string(ctc_ui->get_train_id()).empty())
    {
        ctc_ui->set_dispatch_success(false);
        ctc_ui->set_manual_dispatch_message("Train Input Missing");
    }
    else if (std::string(ctc_ui->get_arrival_time()).empty())
    {
        ctc_ui->set_dispatch_success(false);
        ctc_ui->set_manual_dispatch_message("Arrival Time Input Missing");
    }
    else
    {
        destination_channel.Send(std::string(ctc_ui->get_arrival_station()));
        train_id_channel.Send(std::string(ctc_ui->get_train_id()));
        arrival_time_channel.Send(std::string(ctc_ui->get_arrival_time()));
    }
}

// Handlers run on the backend thread
static void manual_dispatch_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    if (destination_channel.DataAvailable() && arrival_time_channel.DataAvailable() && train_id_channel.DataAvailable())
    {
        // TODO need a function that instantiates a new train model and train controller with the same id
        std::string train_id     = train_id_channel.Receive();
        std::string station_name = destination_channel.Receive();
        std::string arrival_time = arrival_time_channel.Receive();
        std::string current_position;
        std::string authority;
        std::string suggested_speed;

        ctc::Station station = ctc_office.GetStationByName(station_name);

        // Dispatch new train
        if (train_id == "New Train")
        {
            // Backend Dispatch
            ctc::Train new_train;
            ctc_office.DispatchToStation(new_train.train_id, static_cast<uint16_t>(station.block_id), arrival_time);

            //Capture Variable to update
            current_position = std::to_string(ctc_office.GetTrainCurrentPosition(new_train.train_id));
            train_id         = std::to_string(new_train.train_id);
            authority        = std::to_string(ctc_office.GetTrainAuthority(new_train.train_id));
            suggested_speed  = std::to_string(static_cast<int>(ctc_office.GetTrainSuggestedSpeed(new_train.train_id)));

            // Update UI
            slint::ComponentWeakHandle<ui::CtcUi> weak_ui_handle(ctc_ui);
            slint::invoke_from_event_loop([weak_ui_handle, train_id, current_position, authority, suggested_speed, station_name] () {
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
                            train_entry->push_back(slint::StandardListViewItem(station_name.c_str()));
                            auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ui.value()->get_train_schedules());
                            received_train_schedules->push_back(train_entry);

                            // Update Train drop down list
                            auto ui_train_ids = std::dynamic_pointer_cast<slint::VectorModel<slint::SharedString>>(ui.value()->get_trains());
                            ui_train_ids->push_back(slint::SharedString(train_id));

                            // Update Train test bench drop down list
                            auto ui_train_tb_ids = std::dynamic_pointer_cast<slint::VectorModel<slint::SharedString>>(ui.value()->get_trains_tb());
                            ui_train_tb_ids->push_back(slint::SharedString(train_id));

                            // Update dispatch status
                            ui.value()->set_dispatch_success(true);
                            ui.value()->set_manual_dispatch_message("Dispatch Successful!");
                        }
                    }
                });
        }
        // Add another destination to existing train
        else
        {
            // Backend Dispatch
            ctc_office.ManualDispatch(std::stoi(train_id), static_cast<uint16_t>(station.block_id));
            // Update UI
            slint::ComponentWeakHandle<ui::CtcUi> weak_ui_handle(ctc_ui);
            slint::invoke_from_event_loop([weak_ui_handle, train_id, station_name] () {
                    if (auto ui = weak_ui_handle.lock())
                    {
                        if (ui.has_value())
                        {
                            // Add next destination to train destination column in TrainSchedule Table UI
                            auto train_schedules_ui = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ui.value()->get_train_schedules());
                            auto train_entry        = std::dynamic_pointer_cast<slint::VectorModel<slint::StandardListViewItem>>(train_schedules_ui->row_data(std::stoi(train_id) - 1).value());
                            auto train_destination  = train_entry->row_data(4).value();
                            train_destination.text  = train_destination.text + ", " + station_name.c_str();
                            train_entry->set_row_data(4, train_destination);
                        }
                    }
                });
        }
    }
}

/*----------------------------------- Block Occupancy and Failure -----------------------------------*/
static inline void send_occupancy_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    std::string occupied_blocks = std::string(ctc_ui->get_block_occupancy());
    bool        occupied        = ctc_ui->get_occupied();
    std::cout << "Occupied Blocks: " << occupied_blocks << std::endl;
    std::vector<types::BlockId>    blocks = TokenizeOccupancyInput(occupied_blocks, ',');
    std::vector<types::BlockState> block_states;
    std::for_each(blocks.begin(), blocks.end(), [&block_states, occupied] (types::BlockId block_id) {
            block_states.emplace_back(block_id, occupied, false);
        });
    block_occupancy_channel.Send(block_states);

    // TODO update UI if necessary
}

static inline void send_failure_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    std::string occupied_blocks = std::string(ctc_ui->get_block_occupancy());
    std::cout << "Occupied Blocks: " << occupied_blocks << std::endl;
    std::vector<types::BlockId>    blocks = TokenizeOccupancyInput(occupied_blocks, ',');
    std::vector<types::BlockState> block_states;
    std::for_each(blocks.begin(), blocks.end(), [&block_states] (types::BlockId block_id) {
            block_states.emplace_back(block_id, true, true);
        });
    block_occupancy_channel.Send(block_states);
    // TODO update UI if necessary
}

static void send_occupancy_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    if (block_occupancy_channel.DataAvailable())
    {
        std::vector<types::BlockState> block_states = block_occupancy_channel.Receive();
        ctc_office.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);
    }
    else if (!ctc_office.GetUpdatedBlocks().empty())
    {
        std::vector<types::BlockId>    updated_blocks = ctc_office.GetUpdatedBlocks();
        std::vector<types::BlockState> block_states;
        std::for_each(updated_blocks.begin(), updated_blocks.end(), [&block_states, ctc_office] (types::BlockId block_id) {
                types::Block block = ctc_office.GetBlockById(block_id);
                block_states.emplace_back(block.block, block.occupied, block.failed);
            });
        ctc_office.ClearUpdatedBlocks();
        UpdateBlockOccupancyUI(ctc_office, ctc_ui, block_states);
    }
}

/*----------------------------------- Maintenance Mode and Block Fix -----------------------------------*/
// Set Block To Maintenance Mode
static inline void set_maintenance_mode_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    std::vector<types::BlockId> blocks = TokenizeOccupancyInput(std::string(ctc_ui->get_block_id()), ',');
    block_maintenance_channel.Send(blocks);
    std::for_each(blocks.begin(), blocks.end(), [] (types::BlockId block_id) {
            std::cout << "Input Maintenance Blocks: " << block_id << std::endl;
        });
}

static void maintenance_mode_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    if (block_maintenance_channel.DataAvailable())
    {
        std::vector<types::BlockId> input_blocks = block_maintenance_channel.Receive();
        std::vector<types::Block>   maintenance_block;
        std::for_each(input_blocks.begin(), input_blocks.end(), [&ctc_office, &maintenance_block] (types::BlockId block_id) {
                ctc_office.SetBlockMaintenanceMode(block_id, true);
                maintenance_block.push_back(ctc_office.GetBlockById(block_id));
            });

        UpdateBlockMaintenanceUI(ctc_ui, maintenance_block);
    }
}

static inline void fix_block_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    std::vector<types::BlockId> blocks = TokenizeOccupancyInput(std::string(ctc_ui->get_block_id()), ',');
    block_fix_channel.Send(blocks);
    std::for_each(blocks.begin(), blocks.end(), [] (types::BlockId block_id) {
            std::cout << "Input Block: " << block_id << std::endl;
        });
}

static void fix_block_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    if (block_fix_channel.DataAvailable())
    {
        std::vector<types::BlockId> input_blocks = block_fix_channel.Receive();
        std::vector<types::Block>   fix_blocks;
        std::for_each(input_blocks.begin(), input_blocks.end(), [&ctc_office, &fix_blocks] (types::BlockId block_id) {
                ctc_office.SetBlockMaintenanceMode(block_id, false);
                fix_blocks.push_back(ctc_office.GetBlockById(block_id));
            });

        UpdateBlockMaintenanceUI(ctc_ui, fix_blocks);
    }
}

/*----------------------------------- Choose File -----------------------------------*/
static inline void choose_file_callback()
{
    choose_file_channel.Send(true);
}

static void choose_file_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    if (choose_file_channel.DataAvailable())
    {
        bool                      choose_file = choose_file_channel.Receive();
        std::string               file_name;
        std::vector<types::Block> blocks;
        types::Error              error    = ctc_office.ChooseFileAndSetTrackLayout(file_name);
        std::vector<ctc::Station> stations = ctc_office.GetStations();
        if (error == types::Error::ERROR_NONE)
        {
            blocks = ctc_office.GetBlocks();
            blocks.erase(blocks.begin());
        }

        std::vector<types::BlockId> default_route = ctc_office.GetDefaultRoute();
        for (types::BlockId block_id : default_route)
        {
            std::cout << "Block: " << block_id << std::endl;
        }

        slint::ComponentWeakHandle<ui::CtcUi> weak_ui_handle(ctc_ui);
        slint::invoke_from_event_loop([weak_ui_handle, blocks, file_name, choose_file, stations, error] () {
                if (auto ui = weak_ui_handle.lock())
                {
                    if (ui.has_value())
                    {
                        if (error == types::Error::ERROR_NONE)
                        {
                            auto block_data_model = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ui.value()->get_block_data());
                            for (const types::Block &block : blocks)
                            {
                                auto block_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
                                block_entry->push_back(slint::StandardListViewItem(std::string(1, block.section).c_str()));
                                block_entry->push_back(slint::StandardListViewItem(std::to_string(block.block).c_str()));
                                block_entry->push_back(slint::StandardListViewItem(block.maintenance ? "Maintenance" : "Open"));
                                if (block.has_switch)
                                {
                                    block_entry->push_back(slint::StandardListViewItem(block.switched ? "Secondary" : "Primary"));
                                }
                                else
                                {
                                    block_entry->push_back(slint::StandardListViewItem("_"));
                                }
                                block_entry->push_back(slint::StandardListViewItem(block.occupied ? "Occupied" : "_"));
                                block_entry->push_back(slint::StandardListViewItem(block.power_failure ? "Failure" : "_"));
                                block_data_model->push_back(block_entry);
                            }
                            ui.value()->set_block_data(block_data_model);
                            ui.value()->set_selected_fileName(file_name.c_str());
                            ui.value()->set_file_chosen(choose_file);

                            auto stations_model = std::dynamic_pointer_cast<slint::VectorModel<slint::SharedString>>(ui.value()->get_stations());
                            for (const ctc::Station &station : stations)
                            {
                                stations_model->push_back(station.station_name.c_str());
                            }
                            ui.value()->set_stations(stations_model);
                        }
                    }
                }
            });
    }
}

/*----------------------------------- Tick Source -----------------------------------*/

static void tick_source_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    static auto last_call_time = std::chrono::steady_clock::now();
    auto        now            = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(now - last_call_time).count() >= 1)
    {
        last_call_time = now;

        std::string                           current_time = ctc_office.GetTimeString();
        slint::ComponentWeakHandle<ui::CtcUi> weak_ui_handle(ctc_ui);
        slint::invoke_from_event_loop([weak_ui_handle, current_time] () {
                if (auto ui = weak_ui_handle.lock())
                {
                    if (ui.has_value())
                    {
                        ui.value()->set_time(current_time.c_str());
                    }
                }
            });
    }
}

/*----------------------------------- Throughput -----------------------------------*/
static void throughput_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    static auto last_call_time = std::chrono::steady_clock::now();
    auto        now            = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::hours>(now - last_call_time).count() >= 1)
    {
        last_call_time = now;

        size_t                                throughput = ctc_office.GetNumTrains();
        slint::ComponentWeakHandle<ui::CtcUi> weak_ui_handle(ctc_ui);
        slint::invoke_from_event_loop([weak_ui_handle, throughput] () {
                if (auto ui = weak_ui_handle.lock())
                {
                    if (ui.has_value())
                    {
                        ui.value()->set_throughput(std::to_string(throughput).c_str());
                    }
                }
            });
    }
}

/*----------------------------------- Simulation Speed -----------------------------------*/

static inline void simulation_speed_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    std::string simulation_speed = std::string(ctc_ui->get_simulation_speed());
    size_t      i                = 0;
    std::string multiplier;
    if (simulation_speed[i] == 'x')
    {
        i++;
        while (i < simulation_speed.length() && std::isdigit(simulation_speed[i]))
        {
            multiplier += simulation_speed[i++];
        }
    }
    std::cout << "Set Simulation Speed Multiplier to: " << multiplier << std::endl;
    simulation_speed_channel.Send(std::stoi(multiplier));
}

static void simulation_speed_handler(ctc::Ctc &ctc_office)
{
    if (simulation_speed_channel.DataAvailable())
    {
        int multiplier = simulation_speed_channel.Receive();
        ctc_office.SetSimulationSpeedMultiplier(multiplier);
    }
}

/*----------------------------------- Switch Position -----------------------------------*/
static inline void set_switch_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    std::vector<types::BlockId> blocks          = TokenizeOccupancyInput(std::string(ctc_ui->get_block_id()), ',');
    bool                        switch_position = ctc_ui->get_switch_state();
    switch_position_channel.Send(switch_position);
    switch_block_channel.Send(blocks);
    std::for_each(blocks.begin(), blocks.end(), [] (types::BlockId block_id) {
            std::cout << "Input Block: " << block_id << std::endl;
        });
}

static void set_switch_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    if (switch_block_channel.DataAvailable() && switch_position_channel.DataAvailable())
    {
        std::vector<types::BlockId> blocks   = switch_block_channel.Receive();
        bool                        switched = switch_position_channel.Receive();
        types::BlockId              updated_block;
        for (types::BlockId block_id : blocks)
        {
            if (ctc_office.GetBlockById(block_id).has_switch == true)
            {
                ctc_office.SetSwitchPosition(block_id, switched);
                updated_block = block_id;

                bool switch_position = ctc_office.GetBlockById(updated_block).switched;

                slint::ComponentWeakHandle<ui::CtcUi> weak_ui_handle(ctc_ui);
                slint::invoke_from_event_loop([weak_ui_handle, updated_block, switch_position] () {
                        if (auto ui = weak_ui_handle.lock())
                        {
                            if (ui.has_value())
                            {
                                auto block_table_ui = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ui.value()->get_block_data());
                                auto block_entry    = std::dynamic_pointer_cast<slint::VectorModel<slint::StandardListViewItem>>(block_table_ui->row_data(updated_block - 1).value());
                                block_entry->set_row_data(3, slint::StandardListViewItem(switch_position ? "Secondary" : "Primary"));
                                ui.value()->set_switch_state_tb(switch_position);
                            }
                        }
                    });
            }
        }
    }
}

/*----------------------------------- Departure Time -----------------------------------*/
static void departure_time_handler(ctc::Ctc &ctc_office)
{
    std::chrono::system_clock::time_point current_time = ctc_office.GetTime();
    std::vector<ctc::Train>               trains       = ctc_office.GetTrains();
    for (ctc::Train train : trains)
    {
        if (train.departure_time <= current_time && !train.dispatched)
        {
            ctc_office.SetTrainDispatched(train.train_id);
            std::vector<types::BlockState> block_states;
            block_states.emplace_back(63, true, false);
            ctc_office.SetBlockStates(types::TrackId::TRACKID_GREEN, block_states);
        }
    }
}

/*----------------------------------- Automatic Dispatch -----------------------------------*/
static inline void automatic_dispatch_callback()
{
    automatic_dispatch_channel.Send(true);
    std::cout << "Automatic Dispatch!" << std::endl;
}

static void automatic_dispatch_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    if (automatic_dispatch_channel.DataAvailable())
    {
        bool automatic_dispatch_signal = automatic_dispatch_channel.Receive();
        std::cout << "Automatic Dispatch Handler Received!" << std::endl;
        types::Error error = ctc_office.AutomaticDispatch();

        if (error == types::Error::ERROR_NONE)
        {
            std::cout << "Automatic Dispatch Successful" << std::endl;
        }
        else
        {
            std::cout << "Automatic Dispatch Failed" << std::endl;
        }

        std::vector<ctc::Train> trains = ctc_office.GetTrains();
        std::cout << "Number of trains dispatched: " << trains.size() << std::endl;
        std::vector<ctc::Station> stations = ctc_office.GetStations();

        slint::ComponentWeakHandle<ui::CtcUi> weak_ui_handle(ctc_ui);
        slint::invoke_from_event_loop([weak_ui_handle, trains, stations, automatic_dispatch_signal] () {
                if (auto ui = weak_ui_handle.lock())
                {
                    if (ui.has_value())
                    {
                        auto received_train_schedules = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ui.value()->get_train_schedules());
                        auto ui_train_ids             = std::dynamic_pointer_cast<slint::VectorModel<slint::SharedString>>(ui.value()->get_trains());
                        std::for_each(trains.begin(), trains.end(), [&received_train_schedules, &ui_train_ids, stations] (ctc::Train train) {
                            // Push new train entry row to TableView component
                            auto train_entry = std::make_shared<slint::VectorModel<slint::StandardListViewItem>>();
                            train_entry->push_back(slint::StandardListViewItem(std::to_string(train.train_id).c_str()));
                            if (train.current_position == 0)
                            {
                                train_entry->push_back(slint::StandardListViewItem("Yard"));
                            }
                            else
                            {
                                train_entry->push_back(slint::StandardListViewItem(std::to_string(train.current_position).c_str()));
                            }
                            train_entry->push_back(slint::StandardListViewItem(std::to_string(train.authority.size()).c_str()));
                            train_entry->push_back(slint::StandardListViewItem(std::to_string(static_cast<uint16_t>(train.suggested_speed)).c_str()));
                            for (ctc::Station station : stations)
                            {
                                if (station.block_id == train.destination_list[0].destination)
                                {
                                    train_entry->push_back(slint::StandardListViewItem(station.station_name.c_str()));
                                }
                            }
                            received_train_schedules->push_back(train_entry);

                            // Update Train drop down list
                            ui_train_ids->push_back(slint::SharedString(std::to_string(train.train_id)));
                        });


                        if (automatic_dispatch_signal == true)
                        {
                            ui.value()->set_automatic_dispatch_message("Automatic Dispatch Successfully");
                        }
                    }
                }
            });
    }
}

/*----------------------------------- Test Bench -----------------------------------*/

static inline void train_tb_callback(slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    train_id_tb_channel.Send(std::string(ctc_ui->get_train_tb()));
}

static void train_tb_handler(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui)
{
    if (train_id_tb_channel.DataAvailable())
    {
        types::TrainId train_tb_id     = static_cast<uint16_t>(std::stoi(train_id_tb_channel.Receive()));
        int            authority       = static_cast<int>(ctc_office.GetTrainAuthority(train_tb_id));
        int            suggested_speed = static_cast<int>(ctc_office.GetTrainSuggestedSpeed(train_tb_id));
        std::cout << "Authority: " << authority << "; Suggested Speed: " << suggested_speed << std::endl;

        slint::ComponentWeakHandle<ui::CtcUi> weak_ui_handle(ctc_ui);
        slint::invoke_from_event_loop([weak_ui_handle, authority, suggested_speed] () {
                if (auto ui = weak_ui_handle.lock())
                {
                    if (ui.has_value())
                    {
                        ui.value()->set_authority(authority);
                        ui.value()->set_suggested_speed(suggested_speed);
                    }
                }
            });
    }
}

/*----------------------------------- Helper Methods -----------------------------------*/

static void UpdateBlockOccupancyUI(ctc::Ctc &ctc_office, slint::ComponentHandle<ui::CtcUi> &ctc_ui, std::vector<types::BlockState> &block_states)
{
    std::vector<ctc::Train>   trains   = ctc_office.GetTrains();
    std::vector<ctc::Station> stations = ctc_office.GetStations();

    // Update UI
    slint::ComponentWeakHandle<ui::CtcUi> weak_ui_handle(ctc_ui);
    slint::invoke_from_event_loop([weak_ui_handle, trains, block_states, stations] () {
            if (auto ui = weak_ui_handle.lock())
            {
                if (ui.has_value())
                {
                    // Update Train Schedule
                    auto train_schedules_ui = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ui.value()->get_train_schedules());
                    std::for_each(trains.begin(), trains.end(), [&train_schedules_ui, stations] (ctc::Train train) {
                        auto train_entry = std::dynamic_pointer_cast<slint::VectorModel<slint::StandardListViewItem>>(train_schedules_ui->row_data(train.train_id - 1).value());
                        train_entry->set_row_data(1, slint::StandardListViewItem(train.current_position == 0 ? "Yard" : std::to_string(train.current_position).c_str()));
                        train_entry->set_row_data(2, slint::StandardListViewItem(std::to_string(train.authority.size()).c_str()));
                        train_entry->set_row_data(3, slint::StandardListViewItem(std::to_string(static_cast<uint16_t>(train.suggested_speed)).c_str()));
                        types::BlockId destination = train.destination_list[CTC_TRAIN_CURRENT_DESTINATION].destination;
                        if (destination == 0)
                        {
                            train_entry->set_row_data(4, slint::StandardListViewItem("Yard"));
                        }
                        else
                        {
                            for (ctc::Station station : stations)
                            {
                                if (station.block_id == destination)
                                {
                                    train_entry->set_row_data(4, slint::StandardListViewItem(station.station_name.c_str()));
                                }
                            }
                        }
                    });

                    // Update Block Table
                    auto block_table_ui = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ui.value()->get_block_data());
                    std::for_each(block_states.begin(), block_states.end(), [&block_table_ui] (types::BlockState block_state) {
                        auto block_entry = std::dynamic_pointer_cast<slint::VectorModel<slint::StandardListViewItem>>(block_table_ui->row_data(block_state.block - 1).value());
                        if (block_state.track_failure == true)
                        {
                            block_entry->set_row_data(5, slint::StandardListViewItem("Failure"));
                        }
                        else
                        {
                            block_entry->set_row_data(4, slint::StandardListViewItem(block_state.occupied ? "Occupied" : "_"));
                        }
                    });
                }
            }
        });
}

static void UpdateBlockMaintenanceUI(slint::ComponentHandle<ui::CtcUi> &ctc_ui, std::vector<types::Block> &blocks)
{
    slint::ComponentWeakHandle<ui::CtcUi> weak_ui_handle(ctc_ui);
    slint::invoke_from_event_loop([weak_ui_handle, blocks] () {
            if (auto ui = weak_ui_handle.lock())
            {
                if (ui.has_value())
                {
                    auto block_table_ui = std::dynamic_pointer_cast<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(ui.value()->get_block_data());
                    std::for_each(blocks.begin(), blocks.end(), [&block_table_ui] (types::Block block) {
                        auto block_entry = std::dynamic_pointer_cast<slint::VectorModel<slint::StandardListViewItem>>(block_table_ui->row_data(block.block - 1).value());
                        block_entry->set_row_data(2, slint::StandardListViewItem(block.maintenance ? "Maintenance" : "Open"));
                        block_entry->set_row_data(5, slint::StandardListViewItem(block.failed ? "Failure" : "_"));
                    });

                    ui.value()->set_maintenance_block(std::to_string(blocks[0].block).c_str());
                    ui.value()->set_maintenance_mode_tb(blocks[0].maintenance);
                    ui.value()->set_switch_state_tb(blocks[0].switched);
                }
            }
        });
}


std::vector<types::BlockId> TokenizeOccupancyInput(const std::string& input, char delimiter)
{
    std::vector<types::BlockId> occupied_blocks;
    std::stringstream           ss(input);
    std::string                 token;

    // Split the string using the delimiter
    while (std::getline(ss, token, delimiter))
    {
        // Remove whitespace and convert to integer
        occupied_blocks.push_back(std::stoi(token));
    }

    return occupied_blocks;
}

}