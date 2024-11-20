//
// Created by User on 24/10/2024.
//

#ifndef PKS_PROJECT_HANDSHAKESTATS_H
#define PKS_PROJECT_HANDSHAKESTATS_H

#include "Packet/header.h"
#include <boost/asio.hpp>
using boost::asio::ip::udp;

enum class HandShake {
    NOT_CONNECTED,
    WAITING,
    CONNECTED,
};

class HandShakeStats {
private:
    std::shared_ptr<udp::socket> socket;
    std::shared_ptr<std::mutex> socket_m;

    int port;

    std::shared_ptr<udp::endpoint> endpoint;
    std::shared_ptr<std::mutex> endpoint_m;

    HandShake step;
    bool connected{};
public:
    HandShakeStats();
    HandShakeStats(std::shared_ptr<udp::socket> socket, std::shared_ptr<std::mutex> socket_m) {
        this->socket = socket;
        this->socket_m = socket_m;

        endpoint = std::make_shared<udp::endpoint>(udp::endpoint(boost::asio::ip::make_address_v4("127.0.0.1"), 0));
        endpoint_m = std::make_shared<std::mutex>();
    };

    HandShake getStep();

    static Header getRequestHeader();
    static Header getAnswerHeader();

    bool isConnected();

    void tryConnect(int port);

    int getPort() const;

    void handleHandShake(std::unique_ptr<Header> header, const udp::endpoint& back_endpoint);

    std::pair<std::shared_ptr<udp::endpoint>, std::shared_ptr<std::mutex>> getEndpoint() const{
        return std::make_pair(endpoint, endpoint_m);
    }
};


#endif //PKS_PROJECT_HANDSHAKESTATS_H
