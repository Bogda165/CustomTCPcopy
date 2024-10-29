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

    Receiver(std::shared_ptr<udp::socket> socket, std::shared_ptr<std::mutex> socket_m);

    virtual std::thread run(int timeout = 0);

    virtual bool onReceive() = 0;
};


#endif //PKS_PROJECT_RECEIVER_H
