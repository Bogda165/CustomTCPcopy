#ifndef PKS_PROJECT_SHOWOBSERVERPIPE_H
#define PKS_PROJECT_SHOWOBSERVERPIPE_H

#include "showObserver.h"
#include <memory>
#include <unordered_map>
#include <utility>
#include <MySocket.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

class ShowObserverPipe : public ShowObserver {
private:
    int port;

    std::shared_ptr<std::unordered_map<int, std::pair<Header, std::variant<Data, FileData>>>> messages;
    std::shared_ptr<std::mutex> messages_m;

    std::ofstream pipe;
    bool connected;

public:
    ShowObserverPipe(std::shared_ptr<std::unordered_map<int, std::pair<Header, std::variant<Data, FileData>>>> messages, std::shared_ptr<std::mutex> messages_m, int port)
            : messages(messages), messages_m(messages_m), connected(false), port(port) {}

    void openPipe() {
        // Check if pipe already exists and create it if not
        auto name = "writeFifo" + std::to_string(port);
        if (mkfifo(name.c_str(), 0666) == -1 && errno != EEXIST) {
            perror("mkfifo");
            throw std::runtime_error("Error while creating a FIFO!");
        }

        // Open the named pipe for writing
        pipe.open(name.c_str(), std::ios::out);
        if (!pipe.is_open()) {
            std::cerr << "Failed to open pipe for writing" << std::endl;
            throw std::runtime_error("Failed to open FIFO for writing");
        }

        connected = true;
    }

    bool func(int message_id, int message_size) override {
        std::lock_guard<std::mutex> lock(*messages_m);
        auto& obj = (*messages)[message_id];
        bool _return = false;

        // Visit the data and check its size
        std::visit([message_id, &_return, message_size, out_fifo = &this->pipe, connected = &this->connected](auto&& _obj) {
            _return = (_obj.dataAmount() == message_size);
            std::cout << "Size while checking: " << _obj.dataAmount() << " == " << message_size << std::endl;
            if (_return) {
                std::cout << "Message with id: " << message_id << std::endl;
                std::cout << _obj.toString() << std::endl;

                if (*connected) {
                    std::cout << "Writing to pipe" << std::endl;

                    // Use pipe << for writing to the pipe
                    (*out_fifo) << _obj.toString() << std::endl;
                    out_fifo->flush();  // Ensure the data is written out immediately
                }
            }
        }, obj.second);

        if (_return) {
            messages->erase(message_id);
        }
        return _return;
    }

    std::shared_ptr<ShowObserver> getSharedPtr() override {
        return shared_from_this();
    }

    ~ShowObserverPipe() = default;
};

#endif // PKS_PROJECT_SHOWOBSERVERPIPE_H