//
// Created by User on 21/10/2024.
//

#include "MySocket.h"
#include "ShowObserver/showObserverPipe.h"

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
    messages = std::make_shared<std::unordered_map<int, std::pair<Header, std::variant<Data, FileData>>>>();
    messages_m = std::make_shared<std::mutex>();

    handShake = std::make_shared<HandShakeStats>(socket, socket_m);
    handShake_m = std::make_shared<std::mutex>();

    showOb = std::make_shared<ShowObserverPipe>(messages, messages_m, port);

    auto hui = this->runSender(handShake->getEndpoint(), 1);
    auto hui2 = this->reSender(1);
    auto hui3 = showOb->run();

    hui.detach();
    hui2.detach();
    hui3.detach();
    this->status = Status::RECV_SEND;

    this->isConnectionAlive = std::make_shared<std::atomic_bool>(true);

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
    int error = std::rand() % 10 + 1;

    if(error == 2) {
        //data[data.size() - 10] = 0;
    }
    //add a header
    std::lock_guard<std::mutex> lock(*socket_m);
    this->socket->send_to(boost::asio::buffer(data), udp::endpoint(boost::asio::ip::make_address("127.0.0.1"), send_port));
}

std::pair<std::shared_ptr<HandShakeStats>, std::shared_ptr<std::mutex>> MySocket::getHandShakeStat() {
    return std::make_pair(handShake, handShake_m);
}

std::pair<std::shared_ptr<std::unordered_map<int, std::pair<Header, std::variant<Data, FileData>>>>, std::shared_ptr<std::mutex>> MySocket::getMessages() {
    return std::make_pair(messages, messages_m);
}

void MySocket::addToContainerLF(std::unique_ptr<Sendable> obj) {
    auto cloned_obj = obj->clone();
    if (!isInContainerLF(std::move(cloned_obj))) {
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
        auto& data = key.second.second;
        std::visit([](auto& data){data.show();}, data);
    }
    std::cout << std::endl;
}

std::pair<std::shared_ptr<udp::socket>, std::shared_ptr<std::mutex>> MySocket::getSocket(){
    return std::make_pair(socket, socket_m);
}

std::shared_ptr<ShowObserver> MySocket::getShowObserver() {
    return showOb;
}

void MySocket::setStatus(Status _status) {
    status = _status;
}

Status MySocket::getStatus() const {
    return this->status;
}

void MySocket::finish() {
    bool cond = true;
    while(cond) {
        cond = false;
        {
            std::lock_guard<std::mutex> lock(*this->container_m);
            for (auto &packet: *container) {
                auto packet_clone = packet->clone();
                std::unique_ptr<Packet> _packet(dynamic_cast<Packet *>(packet.release()));

                if (_packet) {
                    if (_packet->getHeader().getSequenceNumber() < 0) {
                        cond = true;
                        continue;
                    }
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    exit(0);
}

std::shared_ptr<std::atomic_bool> MySocket::getIsConnectedStatus() {
    return isConnectionAlive;
}