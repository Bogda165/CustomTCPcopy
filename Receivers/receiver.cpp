//
// Created by User on 24/10/2024.
//

#include "receiver.h"
Receiver::Receiver() = default;

Receiver::Receiver(std::shared_ptr<udp::socket> socket, std::shared_ptr<std::mutex> socket_m) {
    this->socket = socket;
    this->socket_m = socket_m;
}

void func(std::shared_ptr<Receiver> receiver, int timeout) {
    std::cout << "starting a loop" << std::endl;
    //maybe add other functionality later
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(timeout));
        if (!receiver->onReceive()) {
            continue;
        }
        std::cout << "received message" << std::endl;
    }
}

std::thread Receiver::run(int timeout) {
    return std::move(std::thread(func, shared_from_this(), timeout));
}