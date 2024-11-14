#include "wayside_controller_port.h"

// TODO remove Unix-specific things
#include <fcntl.h>  // For open()
#include <unistd.h> // For read(), write(), close()

#include <iostream>
#include <string>

#include "logger.h"

namespace wayside_controller
{

static const std::string kLogTag("WAYSIDE CONTROLLER PORT");

WaysideControllerPort::WaysideControllerPort(const char *const port_name)
{
    file_descriptor_ = open(port_name, O_RDWR | O_NOCTTY | O_NDELAY);

    if (-1 == file_descriptor_)
    {
        LOGGER_LOG_DEBUG(std::cout, kLogTag, "Error opening serial port");
    }
    else
    {
        tcgetattr(file_descriptor_, &tty_);

        // Set baud rate
        cfsetospeed(&tty_, B115200);
        cfsetispeed(&tty_, B115200);

        // Set other port parameters (8N1, no flow control)
        tty_.c_cflag &= ~PARENB;     // No parity
        tty_.c_cflag &= ~CSTOPB;     // 1 stop bit
        tty_.c_cflag &= ~CSIZE;
        tty_.c_cflag |= CS8;          // 8 data bits
        tty_.c_cflag &= ~CRTSCTS;     // No flow control

        tcsetattr(file_descriptor_, TCSANOW, &tty_);

        connected_ = true;

        LOGGER_LOG_DEBUG(std::cout, kLogTag, "Serial port opened");
    }
}

WaysideControllerPort::~WaysideControllerPort(void)
{
    if (connected_)
    {
        close(file_descriptor_);

        LOGGER_LOG_DEBUG(std::cout, kLogTag, "Serial port closed");
    }
}

size_t WaysideControllerPort::Send(const uint8_t *const buffer, const size_t size)
{
    int bytes_written = 0;

    if (connected_)
    {
        bytes_written = write(file_descriptor_, buffer, size);

        if (bytes_written < 0)
        {
            bytes_written = 0;
        }
        else
        {
            LOGGER_LOG_DEBUG(std::cout, kLogTag, "Bytes sent: {}", bytes_written);
        }
    }

    return static_cast<size_t>(bytes_written);
}

size_t WaysideControllerPort::SendAvailable(void)
{
    // TODO
    return buffer_.Capacity();
}

size_t WaysideControllerPort::Receive(uint8_t *const buffer, const size_t size)
{
    int bytes_read = 0;

    if (connected_)
    {
        bytes_read = read(file_descriptor_, buffer, size);

        if (bytes_read < 0)
        {
            bytes_read = 0;
        }
        else
        {
            LOGGER_LOG_DEBUG(std::cout, kLogTag, "Bytes read: {}", bytes_read);
        }
    }

    return static_cast<size_t>(bytes_read);
}

size_t WaysideControllerPort::ReceiveAvailable(void)
{
    // TODO
    return buffer_.Capacity();
}

bool WaysideControllerPort::Connected(void)
{
    return connected_;
}

} // namespace wayside_controller