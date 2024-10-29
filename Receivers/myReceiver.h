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

class MyReceiver: public Receiver  {
private:
    std::shared_ptr<std::unordered_map<int, std::pair<Header, Data>>> messages;
    std::shared_ptr<std::mutex> messages_m;

    std::shared_ptr<HandShakeStats> handShake;
    std::shared_ptr<std::mutex> handShake_m;
public:
    MyReceiver(std::shared_ptr<HandShakeStats> handShake, std::shared_ptr<std::mutex> handShake_m,
               std::shared_ptr<std::unordered_map<int, std::pair<Header, Data>>> messages, std::shared_ptr<std::mutex> messages_m,
               std::shared_ptr<udp::socket> socket, std::shared_ptr<std::mutex> socket_m);

    MyReceiver();

    bool onReceive() override;

    void showMessages();

    void getMessage(int n);
};


#endif //PKS_PROJECT_MYRECEIVER_H
