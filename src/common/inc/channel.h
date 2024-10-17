/*****************************************************************************
* @file channel.h
*
* @brief Implements thread safe communication channel.
*****************************************************************************/
#ifndef TRAINS_SRC_COMMON_INC_CHANNEL_H
#define TRAINS_SRC_COMMON_INC_CHANNEL_H

#include <atomic>
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
        bool DataAvailable(void);

    private:
        std::binary_semaphore send_signal_;
        std::binary_semaphore receive_signal_;
        std::atomic_bool data_available_;
        T data_;
};

template <typename T>
Channel<T>::Channel(void) : send_signal_(0), receive_signal_(1), data_available_(false)
{
}

template <typename T>
void Channel<T>::Send(T data)
{
    receive_signal_.acquire();
    data_ = data;
    data_available_.store(true);
    send_signal_.release();
}

template <typename T>
T Channel<T>::Receive(void)
{
    T data;

    send_signal_.acquire();
    data = data_;
    data_available_.store(false);
    receive_signal_.release();

    return data;
}

template <typename T>
bool Channel<T>::DataAvailable(void)
{
    return data_available_.load();
}

#endif // TRAINS_SRC_COMMON_INC_CHANNEL_H