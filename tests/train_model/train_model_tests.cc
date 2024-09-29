/*****************************************************************************
 * @file train_model_tests.cc
 *
 * @brief Short-term testing for the Train Model backend.
 *****************************************************************************/

#include "train_model.h"
#include "train_model_ui.h"

int main(void)
{
    auto train_model_ui = ui::TrainModelUi::create();

    train_model_ui->run();

    return 0;
}