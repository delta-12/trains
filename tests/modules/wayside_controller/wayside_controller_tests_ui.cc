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
/*

std::string ExtractFileName(const std::string& full_path) {
    // Find the last occurrence of backslash
    size_t pos = full_path.find_last_of("\\/");
    if (pos != std::string::npos) {
        return full_path.substr(pos + 1); // Return everything after the last backslash
    } else {
        return full_path; // If no backslash is found, return the full string (it may already be a file name)
    }
}

*/ 
int main(void)
{
    auto wayside_controller_ui = ui::WaysideControllerUi::create();

    
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

    //////////////////////////// 

    /*
    std::string input_filepath = "";
    wayside_controller_ui->select-file([&](){
    #ifdef WIN32
            OPENFILENAME ofn;       // Common dialog box structure
            char file_name[MAX_PATH]; // Buffer for the file name
            ZeroMemory(&ofn, sizeof(ofn)); // Clear the structure
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = nullptr; // If using in a GUI, set to the parent window handle
            ofn.lpstrFile = file_name;
            ofn.lpstrFile[0] = '\0';  // Initialize buffer to empty string
            ofn.nMaxFile = sizeof(file_name);
            ofn.lpstrFilter = "CSV Files\0*.CSV\0";
            ofn.nFilterIndex = 1; // Default to the first filter (Excel)
            ofn.lpstrFileTitle = nullptr;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = nullptr; // Default directory
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            // Open the file dialog
            if (GetOpenFileName(&ofn)) {
                std::cout << "Selected file: " << ofn.lpstrFile << std::endl;
                input_file_path = ofn.lpstrFile;
                ctc_ui->set_selected_fileName(ExtractFileName(ofn.lpstrFile).c_str());
                std::cout << "Input File Path: " + input_file_path << std::endl;
            } else {
                std::cout << "No file selected." << std::endl;
                ctc_ui->set_selected_fileName("No file selected");
            }
    #else
                std::cout << "Operation not supported" << std::endl;
    #endif
        });

        //setter getter for suggested spd from ctc
        wayside_controller_ui->on_disp-sugg-auth([&](){


        });

        //setter getter for sugg auth from ctc
        wayside_controller_ui->on_disp-sugg-auth([&](){

            std::string sauth = "10";
            wayside_controller_ui->set_disp-sugg-auth(sauth.c_str());
            wayside_controller_ui->get_disp-sugg-auth(sauth.c_str());

        });
        
        //pick maintenance mode
        wayside_controller_ui->on_disp-maintenance-md([&](){

            std::string sauth = "10";
            wayside_controller_ui->set_disp-sugg-auth(sauth.c_str());

        });
        */

        //select switch state
        wayside_controller_ui->on_disp-sw-state([&](){

            if (wayside_controller_ui->get_str_sw_statein() == "OPEN")
                wayside_controller_ui->set_str_sw_state("CLOSED");
            else
                wayside_controller_ui->set_str_sw_state("OPEN");

        });

/*
        //select block occ or not
        wayside_controller_ui->on_disp-block-occ([&](){

            std::string sauth = "10";
            wayside_controller_ui->set_disp-sugg-auth(sauth.c_str());

        });

        //select which plc is live
        wayside_controller_ui->on_disp-block-occ([&](){

            std::string sauth = "10";
            wayside_controller_ui->set_disp-sugg-auth(sauth.c_str());

        });

        wayside_controller_ui->on_disp-parse-file([&](){

            std::string sauth = "10";
            wayside_controller_ui->set_disp-sugg-auth(sauth.c_str());

        });
*/

           // wayside_controller_ui->on_set-sugg-spd(comm-spd.c_str());


    wayside_controller_ui->run();
    //worker_thread.join();

    return 0;
}