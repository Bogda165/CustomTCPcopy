//
// Created by User on 21/10/2024.
//

#include "MySocket.h"

const std::string &MySocket::getIp() {
    return ip;
}


int MySocket::getPort() const {
    return port;
}

void MySocket::setIp(const std::string &ip) {
    MySocket::ip = ip;
}

void MySocket::setPort(int recvPort) {
    port = recvPort;
}


MySocket::MySocket(int port, std::string ip, boost::asio::io_context& io_context) {
    this->port = port;
    this->ip = ip;
    this->handShake = HandShake::Step1;

    //bindin sockets
    this->socket = std::make_shared<udp::socket>(udp::socket(io_context, udp::endpoint(boost::asio::ip::make_address(ip), port)));
}
void MySocket::setStep(HandShake step) {
    this->handShake = step;
}

void MySocket::send_to(std::string send_ip, int port, std::vector<uint8_t>& byte_buffer, bool flag) {
    //add a header
    std::lock_guard<std::mutex> lock(this->socket_m);
    this->socket->send_to(boost::asio::buffer(byte_buffer),
                               udp::endpoint(boost::asio::ip::make_address(send_ip), port));
}

bool MySocket::header_handler(std::vector<uint8_t>& buffer, udp::endpoint back_endpoint) {
    int ask = buffer[0];
    int syn = buffer[1];
    std::cout << "C";
    std::cout << ask << syn << std::endl;

    if (this->handShake == HandShake::Step1 && ask == 0 && syn == 1) {
        std::cout << "step 1" << std::endl;
        uint8_t _buffer[3] = {1, 1};
        {
            std::lock_guard<std::mutex> lock(this->socket_m);
            this->socket->send_to(boost::asio::buffer(_buffer), back_endpoint);
        }
        std::cout << "set a step to 3" << std::endl;
        this->handShake = HandShake::Step3;
        // send ask + syn
    }else if (this->handShake == HandShake::Step2 && ask == 1 && syn == 1) {
        std::cout << "step2" << std::endl;
        // send ask
        uint8_t _buffer[2] = {1, 0};
        {
            std::lock_guard<std::mutex> lock(this->socket_m);
            this->socket->send_to(boost::asio::buffer(_buffer), back_endpoint);
        }
        std::cout << "set a step to 2" << std::endl;
    }else if (this->handShake == HandShake::Step3 && ask == 1 && syn == 0) {
        std::cout << "step 3" << std::endl;
        //set connected to true
        this->connected = true;
    }else {
        //can write a buffer
        return false;
    }

    return true;
}

bool MySocket::receive(std::vector<uint8_t>& buffer) {
    char recv_buffer[1024];
    udp::endpoint sender_endpoint;
    size_t length;
    {
        if (socket->available() > 0) {
            std::lock_guard<std::mutex> lock(this->socket_m);
            length = this->socket->receive_from(boost::asio::buffer(recv_buffer), sender_endpoint);
        }else {
            return false;
        }
    }
    std::cout << "Full recv buffer: ";
    for (int i = 0; i < length; i++) {
        std::cout << static_cast<int>(recv_buffer[i]);
    }
    std::cout << std::endl;

    std::vector<uint8_t> header(recv_buffer, recv_buffer + 2);


    //todo get a header
    //now 2 flags are hardcoded as a header as 2 bytes first one is ask second is syn(
    if (header_handler(header, sender_endpoint)) {
        return false;
    };
    //todo delete a header

    buffer = std::vector<uint8_t>(recv_buffer + 2, recv_buffer + length);

    return true;
}

bool MySocket::isConnected() {
    return this->connected;
}