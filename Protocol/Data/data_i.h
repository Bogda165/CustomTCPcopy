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
    static int chunk_len;

    Buffer buffer;
    int data_len;

public:
    Data_i() {
        buffer = Buffer();
        data_len = 0;
    }

    static int getChunkLen() {
        return chunk_len;
    }

    int getDataLen() const {
        return data_len;
    }

    virtual std::vector<uint8_t> getData() const = 0;
    virtual void addChunk(int n, std::vector<uint8_t> chunk) = 0;
    virtual std::vector<Packet> getDataByPackets() = 0;

    virtual void forEachPacket(std::function<void(std::vector<uint8_t>, int)> func) = 0;

    virtual void show() const = 0;

    virtual ~Data_i() = default;
};

template <typename Buffer, typename Value, typename Key, typename Packet>
int Data_i<Buffer, Value, Key, Packet>::chunk_len = 800;


#endif //PKS_PROJECT_DATA_I_H
