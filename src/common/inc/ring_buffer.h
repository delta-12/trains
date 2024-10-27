/*****************************************************************************
* @file ring_buffer.h
*
* @brief Implement a ring buffer.
*****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_RING_BUFFER_H
#define TRAINS_SRC_COMMON_INC_RING_BUFFER_H

#include <array>
#include <cstddef>

template<typename T, size_t buffer_size>
class RingBuffer
{
    public:
        RingBuffer(void);
        size_t Write(const T*const data, const size_t size);
        size_t Read(T*const data, const size_t size);
        size_t Size(void);
        size_t Capacity(void);
        void Clear(void);

    private:
        std::array<T, buffer_size> buffer_;
        size_t write_counter_;
        size_t read_pointer_;
};

template<typename T, size_t buffer_size>
RingBuffer<T, buffer_size>::RingBuffer(void) : write_counter_(0), read_pointer_(0)
{
}

template<typename T, size_t buffer_size>
size_t RingBuffer<T, buffer_size>::Write(const T*const data, const size_t size)
{
    size_t write_count = 0;

    if (nullptr == data)
    {
        // Do not deference nullptr, do nothing
    }
    else if (buffer_.size() == write_counter_)
    {
        // Buffer full, do nothing
    }
    else
    {
        while ((write_counter_ < buffer_.size()) && (write_count < size))
        {
            buffer[(read_pointer_ + write_counter_) % buffer_.size()] = *(data + write_count);

            write_count++;
            write_counter_++;
        }
    }

    return write_count;
}

template<typename T, size_t buffer_size>
size_t RingBuffer<T, buffer_size>::Read(T*const data, const size_t size)
{
    size_t read_count = 0;

    if (nullptr == data)
    {
        // Do not deference nullptr, do nothing
    }
    else
    {
        while ((write_counter_ > 0) && (read_count < size))
        {
            *(data + read_count) = buffer[read_pointer_];

            read_pointer_ = (read_pointer_ + 1) % buffer_size;
            write_counter_--;
            read_count++;
        }
    }

    return read_count;
}


template<typename T, size_t buffer_size>
size_t RingBuffer<T, buffer_size>::Size(void)
{
    return write_counter_;
}

template<typename T, size_t buffer_size>
size_t RingBuffer<T, buffer_size>::Capacity(void)
{
    return buffer_.size();
}

template<typename T, size_t buffer_size>
void RingBuffer<T, buffer_size>::Clear(void)
{
    write_counter_ = 0;
    read_pointer_  = 0;
}

#endif // TRAINS_SRC_COMMON_INC_RING_BUFFER_H