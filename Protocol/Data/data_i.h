//
// Created by User on 06/11/2024.
//

#ifndef PKS_PROJECT_DATA_I_H
#define PKS_PROJECT_DATA_I_H

#include <iterator>
#include <type_traits>
#include <iostream>
#include <Packet/Packet.h>

template<typename Buffer>
class Data_i {
    static_assert(std::is_same<typename std::iterator_traits<typename Buffer::iterator>::value_type, uint8_t>::value,
                  "Container must hold uint_8");

protected:
    static int chunk_len;

    Buffer buffer;
    int data_len;

public:
    Data_i() {
        buffer = std::vector<uint8_t>();
        data_len = 0;
    }

    static int getChunkLen() {
        return chunk_len;
    }

    int getDataLen() const {
        return data_len;
    }

    virtual std::vector<uint8_t> getData() = 0;
    virtual void addChunk(int n, std::vector<uint8_t> chunk) = 0;
    virtual std::vector<std::vector<uint8_t>> getDataByPackets() = 0;

    void forEachPacket(std::function<void(std::vector<uint8_t>, int)> func) {
        auto packets = this->getDataByPackets();
        int packet_id = 0;
        for (auto packet: packets) {
            func(std::move(packet), packet_id);
            packet_id ++;
        }
    }

    virtual void show() const {
        for (auto i: buffer) {
            std::cout << static_cast<int>(i) << " ";
        }
    }
};


#endif //PKS_PROJECT_DATA_I_H
