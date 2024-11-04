// WindowF.h
#ifndef PKS_PROJECT_WINDOWF_H
#define PKS_PROJECT_WINDOWF_H
#include <functional>
#include <chrono>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <stdexcept>
#include "../Exceptions/out_of_range_exception.h"
using tp = std::chrono::system_clock::time_point;

template<typename HUI>
struct Comporator {
    bool operator()(const HUI& lhs, const HUI& rhs) const {
        return lhs > rhs; // Change this to customize the ordering
    }
};

template<class T>
class WindowF {

protected:

    std::unique_ptr<std::unordered_map<int, std::pair<tp, std::unique_ptr<T>>>> buffer;
    std::unique_ptr<std::mutex> buffer_m;
    std::shared_ptr<std::pair<int, int>> indexes;
    std::shared_ptr<std::mutex> indexes_m;
    std::unique_ptr<std::priority_queue<int, std::vector<int>, Comporator<int>>> indexes_b;
    std::unique_ptr<std::mutex> indexes_b_m;

public:

    static int max_buffer_size;
    WindowF()
            : buffer(std::make_unique<std::unordered_map<int, std::pair<tp, std::unique_ptr<T>>>>()),
              buffer_m(std::make_unique<std::mutex>()),
              indexes_b(std::make_unique<std::priority_queue<int, std::vector<int>, Comporator<int>>>()),
              indexes_b_m(std::make_unique<std::mutex>()),
              indexes(std::make_shared<std::pair<int, int>>(0, max_buffer_size)),
              indexes_m(std::make_shared<std::mutex>()) {
    }

    OUT_OF_RANGE whichOutOfRangeLF (int seq_n) {
        if (seq_n < indexes->first) {
            return OUT_OF_RANGE::LEFT;
        }else if (seq_n > indexes->second) {
            return OUT_OF_RANGE::RIGHT;
        }else {
            return OUT_OF_RANGE::IN;
        }
    }

    void calibrateIndexes(){
        int temp = 0;
        std::scoped_lock lock(*indexes_m, *indexes_b_m);

        while (!indexes_b->empty() && indexes_b->top() == indexes->first) {
            indexes_b->pop();
            indexes->first += 1;
        }

        indexes->second = indexes->first + max_buffer_size;

    }

    void addToBuffer(int seq_n, std::unique_ptr<T> obj) {
        std::scoped_lock lock(*indexes_m, *buffer_m);
        if(seq_n < 0) {
            //priority message
            return;
        }if (seq_n >= indexes->first && seq_n <= indexes->second) {
            if (buffer->find(seq_n) == buffer->end()) {
                buffer->insert(std::make_pair(seq_n, std::make_pair(std::chrono::system_clock::now(), std::move(obj))));
            }else {
                std::cout << "The value was already in the window" << std::endl;
            }
        } else {
            auto type = whichOutOfRangeLF(seq_n);
            throw out_of_range_exc(type,  "out of range " + std::to_string(static_cast<int>(type)));
        }
    }

    std::unique_ptr<T> getFromBuffer(int seq_n) {
        std::unique_lock lock(*indexes_m);
        std::unique_lock lock3(*indexes_b_m);
        std::unique_lock<std::mutex> lock2(*buffer_m);

        auto type = whichOutOfRangeLF(seq_n);

        if (seq_n < 0) {
            //priority messsage
            std::cout << "Priority message received!!!" << std::endl;
            std::cout << "sequence number: " << seq_n << std::endl;
            throw std::runtime_error("I mean wtf find me and kill plz");
        }
        if (seq_n >= indexes->first && seq_n < indexes->second) {
            auto it = buffer->find(seq_n);
            if (it != buffer->end()) {
                auto obj = std::move(it->second.second);
                buffer->erase(it);

                indexes_b->push(seq_n);
                lock.unlock();
                lock2.unlock();
                lock3.unlock();
                this->calibrateIndexes();

                return std::move(obj);
            }
        }

        throw out_of_range_exc(type,  "out of range " + std::to_string(static_cast<int>(type)));
    }
};


#endif //PKS_PROJECT_WINDOWF_H