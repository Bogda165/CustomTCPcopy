//
// Created by User on 24/10/2024.
//

#include "HandShakeStats.h"

#include <memory>


HandShakeStats::HandShakeStats() {
    step = HandShake::NOT_CONNECTED;
    connected = false;
}

HandShake HandShakeStats::getStep() {
    return step;
}

bool HandShakeStats::isConnected() {
    return connected;
}

int HandShakeStats::getPort() const {
    return this->port;
}

void HandShakeStats::tryConnect(int _port) {
    this->step = HandShake::WAITING;
    auto _header = Header(Flags::SYN);
    auto sender = udp::endpoint(boost::asio::ip::make_address("127.0.0.1"), _port);
    {
        std::lock_guard<std::mutex> lock((*socket_m));
        socket->send_to(boost::asio::buffer(_header.toU8()), sender);
    }
}

void HandShakeStats::handleHandShake(std::unique_ptr<Header> header, udp::endpoint const& back_endpoint) {
    if (this->step == HandShake::NOT_CONNECTED && !header->getFlags(Flags::ACK) && header->getFlags(Flags::SYN)) {
        std::cout << "step 1" << std::endl;
        Header _header = Header(Flags::ACK, Flags::SYN);
        //also set a data to 0
        {
            std::lock_guard<std::mutex> lock((*socket_m));
            socket->send_to(boost::asio::buffer(_header.toU8()), back_endpoint);
        }
        this->step = HandShake::WAITING;
        // send ask + syn
    }else if (this->step == HandShake::WAITING && header->getFlags(Flags::ACK) && header->getFlags(Flags::SYN)) {
        std::cout << "step2" << std::endl;
        // send ask
        Header _header = Header(Flags::ACK);
        {
            std::lock_guard<std::mutex> lock(*socket_m);
            this->socket->send_to(boost::asio::buffer(_header.toU8()), back_endpoint);
        }
        std::cout << "set a step to 2" << std::endl;
        this->step = HandShake::CONNECTED;
        this->port = back_endpoint.port();
        {
            std::lock_guard<std::mutex> lock(*endpoint_m);
            endpoint = std::make_shared<udp::endpoint>(boost::asio::ip::make_address_v4("127.0.0.1"), port);
        }
        this->connected = true;
    }else if (this->step == HandShake::WAITING && header->getFlags(Flags::ACK) && !header->getFlags(Flags::SYN)) {
        this->step = HandShake::CONNECTED;
        std::cout << "step 3" << std::endl;
        //set connected to true
        this->connected = true;
        this->port = back_endpoint.port();
        {
            std::lock_guard<std::mutex> lock(*endpoint_m);
            endpoint = std::make_shared<udp::endpoint>(boost::asio::ip::make_address_v4("127.0.0.1"), port);
        }
    }
}

Header HandShakeStats::getRequestHeader() {
    Header(0, 0, Flags::ACK);
}

Header HandShakeStats::getAnswerHeader() {
    Header(Flags::ACK, Flags::SYN);
}


