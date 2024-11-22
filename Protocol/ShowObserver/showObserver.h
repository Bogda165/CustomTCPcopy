//
// Created by User on 20/11/2024.
//

#ifndef PKS_PROJECT_SHOWOBSERVER_H
#define PKS_PROJECT_SHOWOBSERVER_H
#include <thread>
#include <chrono>
#include <mutex>
#include <queue>

class ShowObserver: public std::enable_shared_from_this<ShowObserver> {
private:
    std::shared_ptr<std::mutex> lock;
    std::queue<std::pair<int, int>> buffer;
public:
    ShowObserver() {
        lock = std::make_shared<std::mutex>();
        buffer = std::queue<std::pair<int, int>>();
    }

    virtual bool func(int message_id, int message_size) = 0;

    void addToBufferLF(int message_id, int message_size) {
        std::lock_guard<std::mutex> _lock(*lock);
        buffer.push(std::make_pair(message_id, message_size));
    }

    std::thread run() {
        return std::thread([obj = this->getSharedPtr()] {
            while(true) {
                {
                    int offset = 0;
                    std::lock_guard<std::mutex> lock(*obj->lock);
                    while (!obj->buffer.empty() - offset) {
                        auto _return = obj->func(obj->buffer.front().first, obj->buffer.front().second);
                        if(_return) {
                            obj->buffer.pop();
                        }else {
                            offset ++;
                        }
                    }
                }
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }
        });
    }

    std::shared_ptr<std::mutex> getLock() {
        return lock;
    }

    virtual std::shared_ptr<ShowObserver> getSharedPtr() = 0;

    virtual ~ShowObserver() = default;
};


#endif //PKS_PROJECT_SHOWOBSERVER_H
