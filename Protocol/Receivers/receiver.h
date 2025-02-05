//
// Created by User on 24/10/2024.
//

#ifndef PKS_PROJECT_RECEIVER_H
#define PKS_PROJECT_RECEIVER_H
#include <iostream>
#include <boost/asio.hpp>
using boost::asio::ip::udp;

class Receiver : public std::enable_shared_from_this<Receiver>{
public:
    std::shared_ptr<udp::socket> socket;
    std::shared_ptr<std::mutex> socket_m;
public:
    Receiver();

    Receiver(std::shared_ptr<udp::socket> socket, std::shared_ptr<std::mutex> socket_m)
            : socket(std::move(socket)), socket_m(std::move(socket_m)) {
        if (!this->socket || !this->socket_m) {
            throw std::invalid_argument("Socket and mutex must not be null");
        }
    }

    void func(std::shared_ptr<Receiver> receiver, int timeout) {
        std::cout << "starting a loop" << std::endl;
        //maybe add other functionality later
        while (true) {
            //std::this_thread::sleep_for(std::chrono::seconds(timeout));
            if (!receiver->onReceive()) {
                continue;
            }
            std::cout << "received message" << std::endl;
        }
    }

    virtual std::thread run(int timeout = 0) {
        auto self = shared_from_this();
        return std::thread([self, timeout]() {
            self->func(self, timeout);
        });
    }

    virtual bool onReceive() = 0;
};





#endif //PKS_PROJECT_RECEIVER_H
