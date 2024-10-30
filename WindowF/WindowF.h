// WindowF.h
#ifndef PKS_PROJECT_WINDOWF_H
#define PKS_PROJECT_WINDOWF_H
#include <functional>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <stdexcept>

template<typename HUI>
struct Comporator {
    bool operator()(const HUI& lhs, const HUI& rhs) const {
        return lhs > rhs; // Change this to customize the ordering
    }
};

template<class T>
class WindowF {

protected:
    std::unordered_map<int, T> buffer;
    std::shared_ptr<std::pair<int, int>> indexes;
    std::priority_queue<int, std::vector<int>, Comporator<int>> indexes_b;

public:

    static int max_buffer_size;
    WindowF(){
        buffer = std::unordered_map<int, T>();

        indexes = std::make_shared<std::pair<int, int>>(std::pair(0, max_buffer_size));
    }

    void calibrateIndexes(){
        while(!indexes_b.empty() && indexes_b.top() == indexes->first) {
            indexes_b.pop();
            indexes->first += 1;
        }
        indexes->second = indexes->first + max_buffer_size;
    }

    void addToBuffer(int seq_n, T obj) {
        if (seq_n >= indexes->first && seq_n <= indexes->second) {
            buffer.insert(std::make_pair(seq_n, std::move(obj)));
        } else {
            throw std::out_of_range("Sequence number is out of range!!!");
        }
    }

    T getFromBuffer(int seq_n) {
        if (seq_n >= indexes->first && seq_n < indexes->second) {
            auto it = buffer.find(seq_n);
            if (it != buffer.end()) {
                T obj = std::move(it->second);
                buffer.erase(it);

                indexes_b.push(seq_n);
                this->calibrateIndexes();

                return obj;
            }
        }

        throw std::out_of_range("Sequence number is out of range!!!");
    }
};


#endif //PKS_PROJECT_WINDOWF_H