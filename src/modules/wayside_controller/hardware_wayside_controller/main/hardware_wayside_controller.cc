#include "wayside_controller.h"
#include "green_line.h"

wayside_controller::Error GetInput(const wayside_controller::InputId input, wayside_controller::IoSignal &signal);

extern "C" void app_main(void)
{

}

wayside_controller::Error GetInput(const wayside_controller::InputId input, wayside_controller::IoSignal &signal)
{
    return wayside_controller::Error::ERROR_INVALID_INPUT;
}