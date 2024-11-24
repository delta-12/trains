#include <thread>

#include <slint.h>

#include "launcher.h"
#include "simulator.h"
#include "ctc.h"
#include "file_explorer.h"

#ifdef _WIN32
#include <windows.h>
#include <iostream>

void AttachConsoleToApp() {
    AllocConsole(); // Allocates a new console
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout); // Redirects stdout to the console
    freopen_s(&fp, "CONOUT$", "w", stderr); // Redirects stderr to the console
    freopen_s(&fp, "CONIN$", "r", stdin);  // Redirects stdin to the console
}
#endif

int main(void)
{
    #ifdef _WIN32
    AttachConsoleToApp(); 
    #endif
    simulator::Simulator world;
    auto                 launcher_ui           = ui::Launcher::create();
    auto                 ctc_ui                = ui::CtcUi::create();
    auto                 wayside_controller_ui = ui::WaysideControllerUi::create();
    auto                 track_model_ui        = ui::TrackModelUi::create();
    auto                 train_model_ui        = ui::TrainModelUi::create();
    auto                 train_controller_ui   = ui::TrainControllerUi::create();

    launcher_ui->on_launch_ctc_window([&]
    {
        ctc_ui->show();
    });
    launcher_ui->on_launch_wayside_controller_window([&]
    {
        wayside_controller_ui->show();
    });
    launcher_ui->on_launch_track_model_window([&]
    {
        track_model_ui->show();
    });
    launcher_ui->on_launch_train_model_window([&]
    {
        train_model_ui->show();
    });
    launcher_ui->on_launch_train_controller_window([&]
    {
        train_controller_ui->show();
    });

    ctc::Ctc ctc_office;
    ctc_ui->on_choose_file([&]
    {
        FileExplorer file_explorer;
        std::filesystem::path path = file_explorer.GetPath();
        std::string file_name = file_explorer.GetFileName();

        std::cout << "Selected Path: " << path << std::endl;
        std::cout << "Selected File: " << file_name << std::endl;
        ctc_office.SetTrackLayout(path);

        auto block_data_model = std::make_shared<slint::VectorModel<std::shared_ptr<slint::Model<slint::StandardListViewItem>>>>(); 
        std::vector<types::Block> blocks = ctc_office.GetBlocks(); 
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
    });

    

    std::thread worker_thread([&]
    {
        // Main backend loop here
    });

    launcher_ui->run();
    worker_thread.join();

    return 0;
}