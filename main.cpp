#include <iostream>
#include <boost/asio.hpp>
#include "MySocket.h"
#include <thread>
#include "data.h"
#include <chrono>

using boost::asio::ip::udp;

void receive_message(std::shared_ptr<MySocket> socket) {
    std::vector<uint8_t> vec;
    while(true) {
        if (!socket->receive(vec)) {
            continue;
        };
        std::string message = std::string(vec.begin(), vec.end());
        std::cout << "Received message: " << message << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}


void send_message(std::shared_ptr<MySocket> socket) {
    std::string message;
    std::string port_s;
    int port;

    std::cout << "Connection to port: ";
    std::cin >> port_s;
    try {
        port = std::stoi(port_s);
    } catch (...) {
        std::cout << "Error while reading a port" << std::endl;
    }

    // send a message with syn keyword
    int _buffer[2] = {1, 0};
    std::vector<uint8_t> vec = {0, 1};
    socket->setStep(HandShake::Step2);
    socket->send_to("127.0.0.1", port, vec, true);

    //todo change

    while(true) {
        //reading a message
        std::cout << "Enter a message: ";
        std::cin >> message;

        std::vector<uint8_t> vec(message.begin(), message.end());
        vec.insert(vec.begin(), 0);
        vec.insert(vec.begin(), 0);

        socket->send_to("127.0.0.1", port, vec, false);
    }
}

int main() {
    int port;

    std::cout << "Enter port: ";
    std::cin >> port;

    boost::asio::io_context io_context;
    auto socket = std::make_shared<MySocket>(port, "127.0.0.1", std::ref(io_context));

    std::thread recv_thread(receive_message, socket);
    //std::thread send_thread(send_message, socket);
    std::string cmd, data;

    while(true) {
        std::cout << "Enter you command: ";
        std::cin >> cmd >> data;
        if (cmd == "connect") {
            //connect
            try {
                port = std::stoi(data);
            } catch (...) {
                std::cout << "Error while reading a port" << std::endl;
            }

            int _buffer[2] = {1, 0};
            std::vector<uint8_t> vec = {0, 1};
            socket->setStep(HandShake::Step2);
            socket->send_to("127.0.0.1", port, vec, true);

        }else if(cmd == "message") {
            std::vector<uint8_t> vec(data.begin(), data.end());
            vec.insert(vec.begin(), 0);
            vec.insert(vec.begin(), 0);

            socket->send_to("127.0.0.1", port, vec, false);
            std::cout << "Sent to port " << port << std::endl;
            //send message
        }else {
            std::cout << "ub" << std::endl;
        };
    }

    recv_thread.join();
    //send_thread.join();

    return 0;
}