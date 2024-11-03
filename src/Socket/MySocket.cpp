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

bool MySocket::isConnected() {
    std::lock_guard<std::mutex> lock(*handShake_m);
    return handShake->isConnected();
}

MySocket::MySocket(int port, std::string ip, boost::asio::io_context& io_context) : MyWindow<std::vector<std::unique_ptr<Sendable>>, Sendable>() {
    this->port = port;
    this->ip = ip;

    // Binding sockets
    socket = std::make_shared<udp::socket>(udp::socket(io_context, udp::endpoint(boost::asio::ip::make_address(ip), port)));
    socket_m = std::make_shared<std::mutex>();

    // Creating a messages hash map
    messages = std::make_shared<std::unordered_map<int, std::pair<Header, Data>>>();
    messages_m = std::make_shared<std::mutex>();

    handShake = std::make_shared<HandShakeStats>(socket, socket_m);
    handShake_m = std::make_shared<std::mutex>();

    auto hui = this->runSender(handShake->getEndpoint(), 10);
    auto hui2 = this->reSender(10);

    hui.detach();
    hui2.detach();
}
void MySocket::send_to(std::string send_ip, int port, std::unique_ptr<Sendable> obj, bool flag) {
    {
        std::lock_guard<std::mutex> lock(*handShake_m);
        if (!flag && !handShake->isConnected()) {
            std::cout << "do not send" << std::endl;
            return;
        }
    }

    //add a header
    std::lock_guard<std::mutex> lock(*socket_m);
    this->socket->send_to(boost::asio::buffer(obj->toU8()),
                               udp::endpoint(boost::asio::ip::make_address(send_ip), port));
}

void MySocket::sendTo(udp::endpoint endpoint, std::vector<uint8_t> data) {
    int send_port = -1;

    {
        std::lock_guard<std::mutex> lock(*handShake_m);
        if (!handShake->isConnected()) {
            std::cout << "do not send" << std::endl;
            return;
        }
        send_port = handShake->getPort();
    }

    //add a header
    std::lock_guard<std::mutex> lock(*socket_m);
    this->socket->send_to(boost::asio::buffer(data), udp::endpoint(boost::asio::ip::make_address("127.0.0.1"), send_port));
}

std::pair<std::shared_ptr<HandShakeStats>, std::shared_ptr<std::mutex>> MySocket::getHandShakeStat() {
    return std::make_pair(handShake, handShake_m);
}

std::pair<std::shared_ptr<std::unordered_map<int, std::pair<Header, Data>>>, std::shared_ptr<std::mutex>> MySocket::getMessages() {
    return std::make_pair(messages, messages_m);
}

void MySocket::addToContainerLF(std::unique_ptr<Sendable> obj) {
    if (!isInContainerLF(std::move(obj->clone()))) {
        container->push_back(std::move(obj));
    }else {
        std::cout << "The value is already in container!!!" << std::endl;
    }
}

std::unique_ptr<Sendable> MySocket::getFromContainerLF() {
    if (container->size() == 0) {
        return nullptr;
    }
    auto element = std::move(container->front());

    container->erase(container->begin());

    return std::move(element);
}

std::unique_ptr<Sendable> MySocket::lookFromContainerLF() {
    if (container->size() == 0) {
        return nullptr;
    }
    auto element = container->front()->clone();

    return std::move(element);
}

bool MySocket::isInContainerLF(std::unique_ptr<Sendable> obj) {
    if (container->size() == 0) {
        return false;
    }
    for (auto & it : *container) {
        if (it == obj) {
            return true;
        }
    }
    return false;
}

int MySocket::size() const {
    return container->size();
}

void MySocket::showMessages() const {
    std::lock_guard<std::mutex> lock(*messages_m);
    std::cout << "Messages: ";
    for (const auto& key : *messages) {
        std::cout << "Key" << key.first;

        std::cout << "Data: ";
        key.second.second.show();
    }
    std::cout << std::endl;
}

std::pair<std::shared_ptr<udp::socket>, std::shared_ptr<std::mutex>> MySocket::getSocket(){
    return std::make_pair(socket, socket_m);
}
