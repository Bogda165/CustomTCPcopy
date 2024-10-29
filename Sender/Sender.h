// Sender.h
#ifndef PKS_PROJECT_SENDER_H
#define PKS_PROJECT_SENDER_H

#include <boost/asio.hpp>
#include <memory>
#include "../Sendable/Sendable.h"
#include <iterator>
#include <type_traits>
#include <iostream>
#include <thread>

enum class SenderState {
    RUNNING,
    BLOCKED,
    STOPPED,
};

using boost::asio::ip::udp;

template<typename Container>
class Sender {
    static_assert(std::is_same<typename std::iterator_traits<typename Container::iterator>::value_type, std::unique_ptr<Sendable>>::value,
                  "Container must hold std::unique_ptr<Sendable>");

protected:
    std::shared_ptr<std::mutex> container_m;
    std::shared_ptr<Container> container;

    SenderState run;
    std::shared_ptr<std::mutex> block_contaier_m;

public:
    Sender(Container c) : container(std::move(c)) {}

    Sender() {
        container = std::make_shared<Container>();
        container_m = std::make_shared<std::mutex>();
        run = SenderState::STOPPED;
        block_contaier_m =  nullptr;
    }

    virtual void sendTo(udp::endpoint endpoint, std::vector<uint8_t>) = 0;

    virtual void addToContainer(std::unique_ptr<Sendable>) = 0;

    virtual std::unique_ptr<Sendable> getFromContainer() = 0;

    void Condition() {

    }

    virtual int size() const = 0;

    void show() const {
        for (const auto& item : container) {
            item->show();
        }
        std::cout << std::endl;
    }

    std::pair<std::shared_ptr<std::mutex>, std::shared_ptr<Container>> getContainer() {
        return std::make_pair(container_m, container);
    }

    void sendToEvery(udp::endpoint endpoint) {
        while (this->size() > 0) {
            {
                this->run = SenderState::BLOCKED;
                std::lock_guard<std::mutex> lock(*block_contaier_m);
                this->run = SenderState::RUNNING;
            }

            if (!Condition()) {
                return;
            }

            std::cout << "Sending a packet" << std::endl;
            auto packet_b = std::move(getFromContainer());

            sendTo(endpoint, packet_b->toU8());
        }
    }


    std::thread runSender(std::pair<std::shared_ptr<udp::endpoint>, std::shared_ptr<std::mutex>> endpoint, int interval_seconds) {
        // Check for nullptr before creating the thread
        if (!endpoint.second || !endpoint.first) {
            throw std::runtime_error("Null pointer in endpoint");
        }

        //mutex for blocking a thread
        block_contaier_m = std::make_shared<std::mutex>(std::mutex());
        run = SenderState::RUNNING;


        return std::thread([this, endpoint, interval_seconds]() {
            try {
                while (true) {
                    udp::endpoint _endpoint;
                    {
                        std::lock_guard<std::mutex> lock(*endpoint.second);
                        _endpoint = *endpoint.first;
                    }

                    this->sendToEvery(_endpoint);

                    // Exit condition could be added here for controlled termination if needed
                    std::this_thread::sleep_for(std::chrono::seconds(interval_seconds));
                }
            } catch (const std::exception& e) {
                std::cerr << "Exception in thread: " << e.what() << std::endl;
            }
        });
    }
};

#endif // PKS_PROJECT_SENDER_H