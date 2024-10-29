#include <iostream>
#include <boost/asio.hpp>
#include "Socket/MySocket.h"
#include <thread>
#include "Data/data.h"
#include "Socket/header.h"
#include <chrono>
#include "WindowF/WindowF.h"

int Data::chunk_len = 10;
int Data::max_buffer_len = 1024;

template<typename T>
int WindowF<T>::max_buffer_size = 5;

using boost::asio::ip::udp;

/*
void receive_message(std::shared_ptr<MySocket> socket) {
    std::vector<uint8_t> vec;
    while(true) {
        if (!socket->receive(vec)) {
            continue;
        }
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
*/

void operation(int n, std::string str, WindowF<std::string>& window_s, WindowF<std::string>& window_a) {
    window_s.addToBuffer(n, str);
    std::cout << "Send " << n << std::endl;
    window_a.addToBuffer(n, str);
    window_a.getFromBuffer(n);
    std::cout << "Send ack " << n << std::endl;
    window_s.getFromBuffer(n);
}

int main() {

    boost::asio::io_context io_context;
    int message_id = 0;

    int _port;
    std::string cmd, data;

    std::cout << "Enter port: ";
    std::cin >> _port;

    MySocket socket(_port, "127.0.0.1", std::ref(io_context));

    auto recv_thread = socket.getReceiver().first->run(2);

    while(true) {
        std::cout << "Enter you command: ";
        std::cin >> cmd >> data;
        if (cmd == "connect") {
            //connect
            try {
                _port = std::stoi(data);
            } catch (...) {
                std::cout << "Error while reading a port" << std::endl;
            }
            // get header
            socket.getHandShakeStat().first->tryConnect(_port);

        }else if(cmd == "message") {
            std::vector<uint8_t> vec(data.begin(), data.end());

            std::cout << "Data readed: ";

            for (auto i: vec) {
                std::cout << static_cast<int>(i) << " ";
            }
            std::cout << std::endl;

            Data message = Data(vec);

            std::cout << "after converting to a data: ";

            message.show_data();
            
            auto packets = message.getDataByPackets();
            
            int packet_id = 0;
            
            for (const auto& packet: packets) {
                Header header;
                header.setPacketId(packet_id);
                header.setMessageId(message_id);
                header.setOffset(packet.size());
                Packet to_send(header, packet);

                to_send.show();
                header.show();

                auto vec = to_send.toU8();

                std::cout << "Buffer";
                for (auto i: vec) {
                    std::cout << static_cast<int>(i) << " ";
                }
                std::cout << std::endl;

                std::unique_ptr<Sendable> obj = std::make_unique<Packet>(to_send);
                
                socket.addToContainer(std::move(obj));
                
                packet_id ++;
            }

            message_id ++;

            std::cout << "Sent to port " << socket.getHandShakeStat().first->getPort() << std::endl;
            //send message
        }else if (cmd == "show"){
            auto socket_m = socket.getMessages();
            std::lock_guard<std::mutex> lock(*socket_m.second);
            for (auto& message: *(socket_m.first)) {
                std::cout << "Message " << message.first << " ";
                std::cout << message.second.second.toString() << std::endl;
            }
        }else {
            std::cout << "ub" << std::endl;
        };
    }

    recv_thread.join();



    return 0;
}