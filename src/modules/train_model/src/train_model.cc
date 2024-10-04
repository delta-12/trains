#include "train_model.h"

int main()
{
    auto ui = TrainModel::create();

    ui->on_request_ebrake([&]{
        if (ui->get_ebrake_status() == "PASS")
            ui->set_ebrake_status("FAIL");
        else
            ui->set_ebrake_status("PASS");
    });
    ui->on_request_engine_fail([&]{
        if (ui->get_engine_status() == "PASS")
            ui->set_engine_status("FAIL");
        else
            ui->set_engine_status("PASS");
    });
    ui->on_request_signal_fail([&]{
        if (ui->get_signal_status() == "PASS")
            ui->set_signal_status("FAIL");
        else
            ui->set_signal_status("PASS");
    });
    ui->on_request_brake_fail([&]{
        if (ui->get_brake_status() == "PASS")
            ui->set_brake_status("FAIL");
        else
            ui->set_brake_status("PASS");
    });

    ui->run();
    return 0;
}
