//
// Created by User on 24/10/2024.
//

#include "myReceiver.h"

// MyReceiver.cpp
MyReceiver::MyReceiver()
        : messages(std::make_shared<std::unordered_map<int, std::pair<Header, Data>>>()),
          messages_m(std::make_shared<std::mutex>()),
          handShake(std::make_shared<HandShakeStats>()),
          handShake_m(std::make_shared<std::mutex>()) {
    // Default constructor implementation
}

MyReceiver::MyReceiver(std::shared_ptr<HandShakeStats> handShake, std::shared_ptr<std::mutex> handShake_m,
                       std::shared_ptr<std::unordered_map<int, std::pair<Header, Data>>> messages, std::shared_ptr<std::mutex> messages_m,
                       std::shared_ptr<udp::socket> socket, std::shared_ptr<std::mutex> socket_m)
        : handShake(handShake), handShake_m(handShake_m), messages(messages), messages_m(messages_m), Receiver(socket, socket_m) {
}

bool MyReceiver::onReceive() {
    // handle what to do with the message detect if its a handshake or whatever
    std::vector<uint8_t> recv_buffer(1024, 0);
    udp::endpoint sender_endpoint;
    size_t length;
    {
        if (socket->available() > 0) {
            std::lock_guard<std::mutex> lock(*socket_m);
            length = socket->receive_from(boost::asio::buffer(recv_buffer), sender_endpoint);
        }else {
            return false;
        }
    }
    std::cout << "Received from the socket" << std::endl;

    std::cout << "Buffer";
    for (auto i: recv_buffer) {
        std::cout << static_cast<int>(i) << " ";
    }
    std::cout << std::endl;

    auto packet = std::make_unique<Packet>();
    packet->fromU8(std::vector(recv_buffer.begin(), recv_buffer.begin() + length));

    packet->show();

    auto header = std::make_unique<Header>(packet->getHeader());

    // chech if we need data: Now 2 main types hand shake and message later add acknolegment and keep alive

    //if Hand shake
    if (header->getFlags(Flags::ACK) || header->getFlags(Flags::SYN)) {
        std::lock_guard<std::mutex> lock(*handShake_m);
        std::cout << "Process handshake" << std::endl;
        handShake->handleHandShake(std::move(header), sender_endpoint);
    }else {
        // just a usual message
        //get message id
        auto message_id = header->getMessageId();
        {
            auto data_b = packet->getChunk();
            //fix
            std::lock_guard<std::mutex> lock(*messages_m);
            auto message = messages->find(message_id);
            if (message == messages->end()) {
                // if not exist create a new message and data
                Data data = Data();
                data.addChunk(header->getPacketId(), data_b);
                data.show_data();
                messages->insert(std::make_pair(message_id, std::make_pair(*header.release(), std::move(data))));
            }else {
                message->second.second.addChunk(header->getPacketId(), data_b);
            }
        }
    }
}



void MyReceiver::showMessages() {
    std::lock_guard<std::mutex> lock(*messages_m);
    std::cout << "Messages: ";
    for (const auto& key : *messages) {
        std::cout << "Key" << key.first;

        std::cout << "Data: ";
        key.second.second.show();
    }
    std::cout << std::endl;
}