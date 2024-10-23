//
// Created by User on 21/10/2024.
//

#ifndef PKS_PROJECT_MYSOCKET_H
#define PKS_PROJECT_MYSOCKET_H
#include <boost/asio.hpp>
#include <mutex>
#include <iostream>
#include <memory>
using boost::asio::ip::udp;

enum class HandShake {
    Step1,
    Step2,
    Step3
};

class MySocket {
private:

    std::shared_ptr<udp::socket> socket;
    std::mutex socket_m;

    std::string ip;
    int port;
    HandShake handShake;
    bool connected;

    bool header_handler(std::vector<uint8_t>& buffer, udp::endpoint back_endpoint);

public:
    MySocket(int port, std::string ip, boost::asio::io_context& io_context);

    const std::string &getIp();

    int getPort() const;

    void setIp(const std::string &ip);

    void setPort(int sendPort);

    bool isConnected();

    void send_to(std::string send_ip, int port, std::vector<uint8_t>& byte_buffer, bool flag);

    bool receive(std::vector<uint8_t>& buffer);

    void setStep(HandShake step);
};


#endif //PKS_PROJECT_MYSOCKET_H
