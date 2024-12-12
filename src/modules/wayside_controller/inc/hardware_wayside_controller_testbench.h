#ifndef HARDWARE_WAYSIDE_CONTROLLER_TESTBENCH_H
#define HARDWARE_WAYSIDE_CONTROLLER_TESTBENCH_H

#include <filesystem>
#include <unordered_map>
#include <vector>

#include <slint.h>
#include "launcher.h"

#include "channel.h"
#include "controller_handler.h"
#include "ctc.h"
#include "logger.h"
#include "track_model.h"
#include "types.h"

namespace wayside_controller
{

static const char *kHardwareWaysideControllerTestbenchLogTag = "HW WC TB";

class HardwareWaysideControllerTestbench : public ctc::Ctc, public track_model::TrackModel
{
    public:
        HardwareWaysideControllerTestbench(slint::ComponentHandle<ui::HardwareWaysideControllerTestbenchUi> testbench_ui) : testbench_ui_(testbench_ui)
        {
            testbench_ui->on_send_track_circuit_data([testbench_ui, this]{
                this->SetSuggestedSpeedsAndAuthority(static_cast<types::BlockId>(testbench_ui->get_block_f()),
                                                     static_cast<types::MetersPerSecond>(testbench_ui->get_speed_f()),
                                                     static_cast<size_t>(testbench_ui->get_authority_f()));
            });

            testbench_ui->on_send_occupancy([testbench_ui, this]{
                this->SetBlockOccupancy(static_cast<types::BlockId>(testbench_ui->get_block_f()), testbench_ui->get_occupied());
            });

            testbench_ui->on_select_block([testbench_ui, this]{
                if (!this->selected_block_channel_.DataAvailable())
                {
                    this->selected_block_channel_.Send(static_cast<types::BlockId>(testbench_ui->get_block_f()));
                }
            });

            testbench_ui->on_program_plc([testbench_ui, this]{
                if (!this->plc_program_channel_.DataAvailable())
                {
                    this->plc_program_channel_.Send(testbench_ui->get_plc_program().data());
                }
            });
        }

        types::Error SetBlockStates(const types::TrackId track, const std::vector<types::BlockState> &block_states) override
        {
            (void)track;
            block_states_ = block_states;

            return types::Error::ERROR_NONE;
        }
        std::vector<types::TrackCircuitData> GetSuggestedSpeedsAndAuthorities(void) const override
        {
            std::vector<types::TrackCircuitData> data = track_circuit_data_;

            HardwareWaysideControllerTestbench *testbench = (HardwareWaysideControllerTestbench *)this;
            testbench->track_circuit_data_.clear();

            return data;
        }

        void SetSuggestedSpeedsAndAuthority(const types::BlockId block, const types::MetersPerSecond speed, const size_t authority)
        {
            types::TrackCircuitData data(types::TrackId::TRACKID_GREEN, block, speed, authority);

            if (!track_circuit_data_channel_.DataAvailable())
            {
                track_circuit_data_channel_.Send(data);
            }
        }
        void SetBlockOccupancy(const types::BlockId block, const bool occupied)
        {
            if (!occupancy_channel_.DataAvailable())
            {
                occupancy_channel_.Send(std::pair<types::BlockId, bool>(block, occupied));
            }
        }

        types::TrackId GetTrackId(void)
        {
            return types::TrackId::TRACKID_GREEN;
        }
        types::Error AddTrainModel(std::shared_ptr<train_model::TrainModel> train)
        {
            (void)train;
            return types::Error::ERROR_NONE;
        }
        std::shared_ptr<train_model::TrainModel> GetTrainModel(const types::TrainId train) const
        {
            (void)train;

            return std::shared_ptr<train_model::SoftwareTrainModel>();
        }
        void GetTrainModels(std::vector<std::shared_ptr<train_model::TrainModel>> &trains)
        {
            (void)trains;
        }
        void Update(void)
        {
            if (track_circuit_data_channel_.DataAvailable())
            {
                track_circuit_data_.emplace_back(track_circuit_data_channel_.Receive());

                LOGGER_LOG_DEBUG(std::cout, kHardwareWaysideControllerTestbenchLogTag, "Track circuit data received from UI");
            }

            if (occupancy_channel_.DataAvailable())
            {
                std::pair<types::BlockId, bool> occupancy = occupancy_channel_.Receive();

                block_occupancies_[occupancy.first] = occupancy.second;

                LOGGER_LOG_DEBUG(std::cout, kHardwareWaysideControllerTestbenchLogTag, "Occupancy data received from UI");
            }

            if (selected_block_channel_.DataAvailable())
            {
                types::BlockId block    = selected_block_channel_.Receive();
                bool           occupied = false;

                if (block_occupancies_.contains(block))
                {
                    occupied = block_occupancies_[block];
                }

                slint::ComponentWeakHandle<ui::HardwareWaysideControllerTestbenchUi> weak_ui_handle(testbench_ui_);
                slint::invoke_from_event_loop([weak_ui_handle, block, occupied]{
                    if (auto ui = weak_ui_handle.lock())
                    {
                        if (ui.has_value())
                        {
                            ui.value()->set_commanded_speed(0);
                            ui.value()->set_commanded_authority(0);
                            ui.value()->set_occupied(occupied);
                        }
                    }
                });
            }
        }
        template<size_t buffer_size>
        void Update(controller_network::ControllerHandler<buffer_size> &controller_handler, const bool connected)
        {
            Update();

            if (plc_program_channel_.DataAvailable())
            {
                bool                  status       = false;
                std::filesystem::path program_path = std::filesystem::current_path() / plc_program_channel_.Receive();

                if (types::Error::ERROR_NONE == controller_handler.ProgramPlc(1, program_path))
                {
                    status = true;
                }

                slint::ComponentWeakHandle<ui::HardwareWaysideControllerTestbenchUi> weak_ui_handle(testbench_ui_);
                slint::invoke_from_event_loop([weak_ui_handle, status]{
                    if (auto ui = weak_ui_handle.lock())
                    {
                        if (ui.has_value())
                        {
                            ui.value()->set_plc_program_status(status);
                        }
                    }
                });

                LOGGER_LOG_DEBUG(std::cout, kHardwareWaysideControllerTestbenchLogTag, "PLC program {} received from UI", program_path.c_str());
            }

            if (connected)
            {
                slint::ComponentWeakHandle<ui::HardwareWaysideControllerTestbenchUi> weak_ui_handle(testbench_ui_);
                slint::invoke_from_event_loop([weak_ui_handle]{
                    if (auto ui = weak_ui_handle.lock())
                    {
                        if (ui.has_value())
                        {
                            ui.value()->set_connected(true);
                        }
                    }
                });
            }
        }

        types::Error SetSwitchState(const types::BlockId block, const bool switched);
        types::Error SetCrossingState(const types::BlockId block, const bool closed);
        types::Error SetRedTrafficLight(const types::BlockId block, const bool on);
        types::Error SetYellowTrafficLight(const types::BlockId block, const bool on);
        types::Error SetGreenTrafficLight(const types::BlockId block, const bool on);
        types::Error SetCommandedSpeed(const types::BlockId block, const types::MetersPerSecond speed);
        types::Error SetAuthority(const types::BlockId block, const types::Blocks authority);
        types::Error GetBlockOccupancy(const types::BlockId block, bool &occupied) const;

    private:
        std::vector<types::BlockState> block_states_;
        std::vector<types::TrackCircuitData> track_circuit_data_;
        std::unordered_map<types::BlockId, bool> block_occupancies_;
        slint::ComponentHandle<ui::HardwareWaysideControllerTestbenchUi> testbench_ui_;
        Channel<types::TrackCircuitData> track_circuit_data_channel_;
        Channel<std::pair<types::BlockId, bool>> occupancy_channel_;
        Channel<types::BlockId> selected_block_channel_;
        Channel<std::string> plc_program_channel_;
};

types::Error HardwareWaysideControllerTestbench::SetSwitchState(const types::BlockId block, const bool switched)
{
    slint::ComponentWeakHandle<ui::HardwareWaysideControllerTestbenchUi> weak_ui_handle(testbench_ui_);

    if (77 == block)
    {
        slint::invoke_from_event_loop([weak_ui_handle, switched]{
                if (auto ui = weak_ui_handle.lock())
                {
                    if (ui.has_value())
                    {
                        ui.value()->set_switch_77(switched);
                    }
                }
            });
    }
    else if (85 == block)
    {
        slint::invoke_from_event_loop([weak_ui_handle, switched]{
                if (auto ui = weak_ui_handle.lock())
                {
                    if (ui.has_value())
                    {
                        ui.value()->set_switch_85(switched);
                    }
                }
            });
    }

    return types::Error::ERROR_NONE;
}

types::Error HardwareWaysideControllerTestbench::SetCrossingState(const types::BlockId block, const bool closed)
{
    slint::ComponentWeakHandle<ui::HardwareWaysideControllerTestbenchUi> weak_ui_handle(testbench_ui_);

    if (108 == block)
    {
        slint::invoke_from_event_loop([weak_ui_handle, closed]{
                if (auto ui = weak_ui_handle.lock())
                {
                    if (ui.has_value())
                    {
                        ui.value()->set_crossing_108(closed);
                    }
                }
            });
    }

    return types::Error::ERROR_NONE;
}

types::Error HardwareWaysideControllerTestbench::SetRedTrafficLight(const types::BlockId block, const bool on)
{
    (void)block;
    (void)on;

    // slint::ComponentWeakHandle<ui::HardwareWaysideControllerTestbenchUi> weak_ui_handle(testbench_ui_);

    // if (76 == block)
    // {
    //     slint::invoke_from_event_loop([weak_ui_handle, on]{
    //             if (auto ui = weak_ui_handle.lock())
    //             {
    //                 if (ui.has_value())
    //                 {
    //                     ui.value()->set_green_light_76(!on);
    //                 }
    //             }
    //         });
    // }
    // else if (100 == block)
    // {
    //     slint::invoke_from_event_loop([weak_ui_handle, on]{
    //             if (auto ui = weak_ui_handle.lock())
    //             {
    //                 if (ui.has_value())
    //                 {
    //                     ui.value()->set_green_light_100(!on);
    //                 }
    //             }
    //         });
    // }

    return types::Error::ERROR_NONE;
}

types::Error HardwareWaysideControllerTestbench::SetYellowTrafficLight(const types::BlockId block, const bool on)
{
    // TODO Update testbench ui
    (void)block;
    (void)on;

    return types::Error::ERROR_NONE;
}

types::Error HardwareWaysideControllerTestbench::SetGreenTrafficLight(const types::BlockId block, const bool on)
{
    slint::ComponentWeakHandle<ui::HardwareWaysideControllerTestbenchUi> weak_ui_handle(testbench_ui_);

    if (76 == block)
    {
        slint::invoke_from_event_loop([weak_ui_handle, on]{
                if (auto ui = weak_ui_handle.lock())
                {
                    if (ui.has_value())
                    {
                        ui.value()->set_green_light_76(on);
                    }
                }
            });
    }
    else if (100 == block)
    {
        slint::invoke_from_event_loop([weak_ui_handle, on]{
                if (auto ui = weak_ui_handle.lock())
                {
                    if (ui.has_value())
                    {
                        ui.value()->set_green_light_100(on);
                    }
                }
            });
    }

    return types::Error::ERROR_NONE;
}

types::Error HardwareWaysideControllerTestbench::SetCommandedSpeed(const types::BlockId block, const types::MetersPerSecond speed)
{
    slint::ComponentWeakHandle<ui::HardwareWaysideControllerTestbenchUi> weak_ui_handle(testbench_ui_);
    slint::invoke_from_event_loop([weak_ui_handle, block, speed]{
            if (auto ui = weak_ui_handle.lock())
            {
                if (ui.has_value())
                {
                    if (static_cast<types::BlockId>(ui.value()->get_block_f()) == block)
                    {
                        ui.value()->set_commanded_speed(speed);

                        LOGGER_LOG_DEBUG(std::cout, kHardwareWaysideControllerTestbenchLogTag, "Setting commanded speed {} m/s", speed);
                    }
                }
            }
        });

    return types::Error::ERROR_NONE;
}

types::Error HardwareWaysideControllerTestbench::SetAuthority(const types::BlockId block, const types::Blocks authority)
{
    slint::ComponentWeakHandle<ui::HardwareWaysideControllerTestbenchUi> weak_ui_handle(testbench_ui_);
    slint::invoke_from_event_loop([weak_ui_handle, block, authority]{
            if (auto ui = weak_ui_handle.lock())
            {
                if (ui.has_value())
                {
                    if (static_cast<types::BlockId>(ui.value()->get_block_f()) == block)
                    {
                        ui.value()->set_commanded_authority(authority);

                        LOGGER_LOG_DEBUG(std::cout, kHardwareWaysideControllerTestbenchLogTag, "Setting commanded authority {} blocks", authority);
                    }
                }
            }
        });

    return types::Error::ERROR_NONE;
}

types::Error HardwareWaysideControllerTestbench::GetBlockOccupancy(const types::BlockId block, bool &occupied) const
{
    types::Error error = types::Error::ERROR_NONE;

    if (!block_occupancies_.contains(block))
    {
        occupied = false;
    }
    else
    {
        occupied = block_occupancies_.find(block)->second;
    }

    return error;
}

} // namespace wayside_controller

#endif