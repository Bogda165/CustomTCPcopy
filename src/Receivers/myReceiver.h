//
// Created by User on 24/10/2024.
//

#ifndef PKS_PROJECT_MYRECEIVER_H
#define PKS_PROJECT_MYRECEIVER_H
#include "receiver.h"
#include <unordered_map>
#include "../Socket/header.h"
#include "../Data/data.h"
#include <iostream>
#include <mutex>
#include <memory>
#include "../Socket/HandShakeStats.h"
#include "../Socket/MySocket.h"

class MyReceiver: public Receiver {
private:
    std::shared_ptr<MySocket> customSocket;
public:
    MyReceiver() = default;
    MyReceiver(std::shared_ptr<MySocket> customSocket): Receiver(customSocket->getSocket().first, customSocket->getSocket().second) {
        this->customSocket = std::move(customSocket);
    };

    void setSocket(std::shared_ptr<MySocket> socket) {
        this->customSocket = socket;
        this->socket = socket->getSocket().first;
        this->socket_m = socket->getSocket().second;
    }

    std::shared_ptr<MySocket> getSocket() {
        return this->customSocket;
    }

    bool onReceive() override;
};


#endif //PKS_PROJECT_MYRECEIVER_H
