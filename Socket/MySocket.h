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
#include "header.h"
#include "../Data/data.h"
#include "HandShakeStats.h"
#include "../Receivers/myReceiver.h"
#include "../Sendable/Sendable.h"
#include "../Sender/Sender.h"
#include <queue>

using boost::asio::ip::udp;

class MySocket: public Sender<std::vector<std::unique_ptr<Sendable>>> {
private:
    // add receive handler
    std::shared_ptr<udp::socket> socket;
    std::shared_ptr<std::mutex> socket_m;

    std::string ip;
    int port;

    //set for messages, clean message, after finished
    //as a key take message id now, and later combine with ip, for multi speaking
    std::shared_ptr<std::unordered_map<int, std::pair<Header, Data>>> messages;
    std::shared_ptr<std::mutex> messages_m;

    //hand shake stats for socket
    std::shared_ptr<HandShakeStats> handShake;
    std::shared_ptr<std::mutex> handShake_m;

    //receiver handler
    std::shared_ptr<MyReceiver> receiver;
    std::shared_ptr<std::mutex> receiver_m;


public:
    MySocket(int port, std::string ip, boost::asio::io_context& io_context);

    const std::string &getIp();

    int getPort() const;

    void setIp(const std::string &ip);

    void setPort(int sendPort);

    bool isConnected();

    void send_to(std::string send_ip, int port, std::unique_ptr<Sendable> obj, bool flag = false);

    std::thread run_receiver();

    std::pair<std::shared_ptr<MyReceiver>, std::shared_ptr<std::mutex>> getReceiver();

    std::pair<std::shared_ptr<HandShakeStats>, std::shared_ptr<std::mutex>> getHandShakeStat();

    std::pair<std::shared_ptr<std::unordered_map<int, std::pair<Header, Data>>>, std::shared_ptr<std::mutex>> getMessages();

    void sendTo(udp::endpoint endpoint, std::vector<uint8_t> data) override;

    virtual void addToContainer(std::unique_ptr<Sendable>) override;

    virtual std::unique_ptr<Sendable> getFromContainer() override;

    virtual int size() const override;
};


#endif //PKS_PROJECT_MYSOCKET_H
