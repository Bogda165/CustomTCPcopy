//
// Created by User on 06/11/2024.
//

#ifndef PKS_PROJECT_DATA_I_H
#define PKS_PROJECT_DATA_I_H

#include <iterator>
#include <type_traits>
#include <iostream>
#include <Packet/Packet.h>

template <typename Buffer, typename Value, typename Key = int, typename Packet = Value>
class Data_i {
    static_assert(
            //std::is_same<typename std::iterator_traits<typename Buffer::iterator>::value_type, Value>::value ||
            std::is_same<Value, uint8_t>::value ||
            // Ensure Value is a std::pair<Key, std::vector<uint8_t>>
            (std::is_same<Value, std::pair<Key, std::vector<uint8_t>>>::value),
            "Container must hold uint8_t or std::pair<Key, std::vector<uint8_t>>"
    );

protected:
    int chunk_len;

    Buffer buffer;
    int data_len;
    int data_amount;

public:
    Data_i() {
        buffer = Buffer();
        data_len = 0;
        data_amount = 0;
        chunk_len = 0;
    }

    int getChunkLen() {
        return chunk_len;
    }

    int setChunkLen(int newlen) {
        chunk_len =  newlen;
    }

    int getDataLen() const {
        return data_len;
    }

    int dataAmount() const {
        return data_amount;
    }

    virtual std::vector<uint8_t> getData() const = 0;
    virtual void addChunk(int n, std::vector<uint8_t> chunk) = 0;
    virtual std::vector<Packet> getDataByPackets() = 0;

    virtual void forEachPacket(std::function<void(std::vector<uint8_t>, int)> func) = 0;

    template <typename... Args>
    void forEachPacketWithArgs(std::function<void(std::vector<uint8_t>, int, Args...)> func, Args... args) {
        forEachPacket([&func, args...](std::vector<uint8_t> vec, int num) {
            func(vec, num, args...);
        });
    }

    virtual void show() const = 0;
    virtual std::string toString() const = 0;

    virtual ~Data_i() = default;
};


#endif //PKS_PROJECT_DATA_I_H
