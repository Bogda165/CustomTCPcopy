//
// Created by User on 21/10/2024.
//

#ifndef PKS_PROJECT_MYSOCKET_H
#define PKS_PROJECT_MYSOCKET_H
#include <boost/asio.hpp>
#include <mutex>
#include <iostream>
#include <memory>
#include <unordered_map>
#include "Packet/header.h"
#include "Data/data.h"
#include "Data/FileData.h"
#include "HandShakeStats.h"
#include "../Sendable/Sendable.h"
#include "../Sender/Sender.h"
#include "../WindowF/MyWindow.h"
#include <queue>
#include <ShowObserver/showObserverTerminal.h>
#include <ShowObserver/showObserver.h>

using boost::asio::ip::udp;

class MySocket: public MyWindow<std::vector<std::unique_ptr<Sendable>>, Sendable> {
private:
    // add receive handler
    std::shared_ptr<udp::socket> socket;
    std::shared_ptr<std::mutex> socket_m;

    std::string ip;
    int port;

    //set for messages, clean message, after finished
    //as a key take message id now, and later combine with ip, for multi speaking
    std::shared_ptr<std::unordered_map<int, std::pair<Header, std::variant<Data, FileData>>>> messages;
    std::shared_ptr<std::mutex> messages_m;

    //hand shake stats for socket
    std::shared_ptr<HandShakeStats> handShake;
    std::shared_ptr<std::mutex> handShake_m;

    std::shared_ptr<ShowObserver> showOb;

public:
    MySocket(int port, std::string ip, boost::asio::io_context& io_context);

    const std::string &getIp();

    int getPort() const;

    void setIp(const std::string &ip);

    void setPort(int sendPort);

    bool isConnected();

    void send_to(std::string send_ip, int port, std::unique_ptr<Sendable> obj, bool flag = false);

    std::thread run_receiver();

    std::pair<std::shared_ptr<HandShakeStats>, std::shared_ptr<std::mutex>> getHandShakeStat();

    std::pair<std::shared_ptr<std::unordered_map<int, std::pair<Header, std::variant<Data, FileData>>>>, std::shared_ptr<std::mutex>> getMessages();

    std::pair<std::shared_ptr<udp::socket>, std::shared_ptr<std::mutex>> getSocket();

    void sendTo(udp::endpoint endpoint, std::vector<uint8_t> data) override;

    virtual void addToContainerLF(std::unique_ptr<Sendable>) override;

    virtual std::unique_ptr<Sendable> getFromContainerLF() override;

    virtual std::unique_ptr<Sendable> lookFromContainerLF() override;

    virtual bool isInContainerLF(std::unique_ptr<Sendable>) override;

    virtual int size() const override;

    void showMessages()const;

    std::shared_ptr<ShowObserver> getShowObserver();
};


#endif //PKS_PROJECT_MYSOCKET_H
