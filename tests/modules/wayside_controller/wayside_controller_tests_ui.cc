/*****************************************************************************
 * @file wayside_controller_tests.cc
 *
 * @brief Short-term testing for the Wayside Controller backend.
 *****************************************************************************/

#include <chrono>
#include <thread>

#include <slint.h>

#include "channel.h"
#include "tick_source.h"
#include "wayside_controller.h"
#include "wayside_controller_ui.h"


int main(void)
{
    auto wayside_controller_ui = ui::WaysideControllerUi::create();

    
    //tick source
    slint::ComponentWeakHandle<ui::WaysideControllerUi> weak_ui_handle(wayside_controller_ui);
    std::thread worker_thread([&]
    {
        TickSource tick_source("12:00:00");
        Channel<std::string> channel;

        tick_source.Start();

        for (std::size_t i = 0; i < 10; i++)
        {
            channel.Send(tick_source.GetTimeString());
            slint::invoke_from_event_loop([weak_ui_handle, &channel]() {
            if (auto ui = weak_ui_handle.lock()) {
                if (ui.has_value())
                {
                    ui.value()->set_time(channel.Receive().c_str());
                }
            }
        });
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }


    });

        //select switch state
        wayside_controller_ui->on_disp_sw_state([&](){

            if (wayside_controller_ui->get_str_sw_statein() == "OPEN")
                wayside_controller_ui->set_str_sw_state("CLOSED");
            else
                wayside_controller_ui->set_str_sw_state("OPEN");

        });



    wayside_controller_ui->run();
    worker_thread.join();

    return 0;
}