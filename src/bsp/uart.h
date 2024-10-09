/*****************************************************************************
* @file uart.h
*
* @brief Defines BSP interface for UART communication.
*****************************************************************************/
#ifndef TRAINS_SRC_BSP_UART_H
#define TRAINS_SRC_BSP_UART_H

#include <array>
#include <cstddef>
#include <cstdint>

namespace bsp
{

typedef uint32_t BaudRate;

typedef enum
{
    PARITY_NONE,
    PARITY_EVEN,
    PARITY_ODD
} Parity;

class UartHandler
{
public:
    virtual bool SetBaudRate(const BaudRate baud_rate) = 0;
    virtual BaudRate GetBaudRate(void) const = 0;
    virtual bool SetPartiy(const Parity parity) = 0;
    virtual Parity GetParity(void) const = 0;
    virtual size_t BytesAvailable(void) const = 0;
    virtual uint8_t Read(void) = 0;
    virtual size_t Read(uint8_t *const buffer, const size_t size) = 0;
    virtual size_t Write(const uint8_t byte) = 0;
    virtual size_t Write(const uint8_t *const buffer, const size_t size) = 0;
};

}

#endif // TRAINS_SRC_BSP_UART_H