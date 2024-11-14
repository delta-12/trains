/*****************************************************************************
* @file wayside_controller_port.h
*
* @brief Implements a UART port for the Wayside Controller.
*****************************************************************************/
#ifndef TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_PORT_H
#define TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_PORT_H

// TODO remove Unix-specific things
#include <termios.h> // For configuring the serial port

#include "ring_buffer.h"
#include "types.h"

namespace wayside_controller
{

class WaysideControllerPort : public types::Port
{
    public:
        WaysideControllerPort(const char *const port_name);
        ~WaysideControllerPort(void);
        size_t Send(const uint8_t *const buffer, const size_t size);
        size_t SendAvailable(void);
        size_t Receive(uint8_t *const buffer, const size_t size);
        size_t ReceiveAvailable(void);
        bool Connected(void);

    private:
        RingBuffer<uint8_t, 2048> buffer_;
        bool connected_ = false;
        termios tty_;
        int file_descriptor_;
};

} // namespace wayside_controller

#endif // TRAINS_SRC_MODULES_WAYSIDE_CONTROLLER_INC_WAYSIDE_CONTROLLER_PORT_H