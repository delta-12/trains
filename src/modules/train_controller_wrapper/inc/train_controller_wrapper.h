#ifndef TRAIN_CONTROLLER_WRAPPER_H
#define TRAIN_CONTROLLER_WRAPPER_H



#include <unistd.h>
#include <cstdint>
#include <iostream>
#include <memory>

#include "train_controller.h"
#include "tick_source.h"
#include "controller_port.h"
#include "controller_network_protocols.h"
#include "ring_buffer.h"

namespace train_controller_wrapper
{
class SoftwarePort : public types::Port
{
    public:
        size_t Send(const uint8_t *const buffer, const size_t size)
        {
            return buffer_.Write(buffer, size);
        }
        size_t SendAvailable(void)
        {
            return (buffer_.Capacity() - buffer_.Size());
        }
        size_t Receive(uint8_t *const buffer, const size_t size)
        {
            return buffer_.Read(buffer, size);
        }
        size_t ReceiveAvailable(void)
        {
            return buffer_.Size();
        }
        bool Connected(void)
        {
            return true;
        }
    
    private:
        RingBuffer<uint8_t, 1024> buffer_;
};

template <size_t buffer_size>
class TrainControllerWrapper
{
    private:
        
        std::shared_ptr<TickSource> clock_;
        train_controller::SoftwareTrainController TC;
        
        std::shared_ptr<controller_network::BasicControllerPort<1024>> controller_port;
        




    public:
        TrainControllerWrapper(std::shared_ptr<TickSource> clk, std::shared_ptr<controller_network::BasicControllerPort<1024>> software_port) : TC(clk), controller_port(software_port)
        {

        }

        void Update()
        {
            uint8_t receive_data[buffer_size];
            controller_network::MessageType  message_type = controller_network::MessageType::MESSAGETYPE_NONE;
            auto message_size = (*controller_port).ReceiveMessage(message_type, receive_data, buffer_size)

            message_type.ParseFromArray(receive_data.data(), message_size)

            TC.SetAuthority(message_type.authority());
            TC.SetPolartity(message_type.polarity());
            TC.SetCurrentSpeed(message_type.speed_meters_per_second());
            TC.SetCommandedSpeed(message_type.current_speed_meters_per_second());

            TC.Update();

            //Include sending data to train Model here
        }

        


};


}







#endif  // TRAIN_CONTROLLER_WRAPPER_H