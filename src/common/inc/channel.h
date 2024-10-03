/*****************************************************************************
* @file channel.h
*
* @brief Implements thread safe communication channel.
*****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_CHANNEL_H
#define TRAINS_SRC_COMMON_INC_CHANNEL_H

#include <semaphore>

template <typename T>
class Channel
{
    public:
        Channel(void);
        Channel(const Channel &)            = delete;
        Channel &operator=(const Channel &) = delete;
        void Send(T data);
        T Receive(void);

    private:
        std::binary_semaphore send_signal_;
        std::binary_semaphore receive_signal_;
        T data_;
};

template <typename T>
Channel<T>::Channel(void) : send_signal_(0), receive_signal_(1)
{
}

template <typename T>
void Channel<T>::Send(T data)
{
    receive_signal_.acquire();
    data_ = data;
    send_signal_.release();
}

template <typename T>
T Channel<T>::Receive(void)
{
    T data;

    send_signal_.acquire();
    data = data_;
    receive_signal_.release();

    return data;
}

#endif // TRAINS_SRC_COMMON_INC_CHANNEL_H