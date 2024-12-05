//
// Created by User on 23/11/2024.
//

#ifndef PKS_PROJECT_KEEPALIVE_H
#define PKS_PROJECT_KEEPALIVE_H
#include <thread>
#include <atomic>
#include <memory>
#include <MySocket.h>

class KeepAlive: public std::enable_shared_from_this<KeepAlive> {
private:
    static int heartBeatAmount;
    std::shared_ptr<MySocket> socket;
    std::shared_ptr<std::atomic<int>> seq_n;
public:

    KeepAlive() = default;

    KeepAlive(std::shared_ptr<MySocket> socket, std::shared_ptr<std::atomic<int>> seq_n) {
        this->socket = socket;
        this->seq_n = seq_n;
    }

    std::thread run() {
        auto self = shared_from_this();
        return std::thread([self] {
            std::unique_ptr<Sendable> keepAliveHeader = std::make_unique<Header>(Flags::KEEP);
            auto isAlive = self->socket->getIsConnectedStatus();
            while(true) {
                if(!self->socket->getHandShakeStat().first->isConnected()) {
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    continue;
                }
                if (isAlive->load(std::memory_order_seq_cst)) {
                    isAlive->store(false, std::memory_order_seq_cst);
                }else {
                    isAlive->store(false, std::memory_order_seq_cst);
                    for (int i = 0; i < heartBeatAmount; i++) {
                        auto hb_clone = keepAliveHeader->clone();
                        self->socket->addToContainer(std::move(hb_clone));
                        std::this_thread::sleep_for(std::chrono::seconds(3));
                        if(isAlive->load(std::memory_order_seq_cst)){ break; }
                    }
                    if(!isAlive->load(std::memory_order_seq_cst)) {
                        std::cout << "End no answer" << std::endl;
                        exit(0);
                        //end the socket
                    }
                }
                auto ka_clone=  keepAliveHeader->clone();
                self->socket->addToContainer(std::move(ka_clone));
                std::cout << "Sending keep alive" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        });
    }


};

int KeepAlive::heartBeatAmount = 3;

#endif //PKS_PROJECT_KEEPALIVE_H
