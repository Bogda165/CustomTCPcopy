//
// Created by User on 23/10/2024.
//

#ifndef PKS_PROJECT_DATA_H
#define PKS_PROJECT_DATA_H
#include <vector>
#include <fstream>
#include <iostream>
#include "../Packet/Packet.h"
#include "data_i.h"

enum class data_type {
    VEC,
    FILE,
};

enum class status {
    EMPTY,
    FILLED,
};

class Data: public Data_i<std::vector<uint8_t>, uint8_t, int, std::vector<uint8_t>> {
private:
    data_type type;
    // file of data
    std::fstream buffer_f;
    //buffer status
    status buffer_s;

    static int max_buffer_len;

    std::vector<int> indexes_b;

    //offset for the last buffer from the rigth side
    int offset;
public:

    // Move constructor
    Data(Data&& other) noexcept
            : Data_i(std::move(other)), type(std::move(other.type)), buffer_f(std::move(other.buffer_f)),
              buffer_s(std::move(other.buffer_s)), indexes_b(std::move(other.indexes_b)), offset(other.offset) {
        // Move the buffer
        buffer = std::move(other.buffer);
    }

    Data& operator=(Data&& other) noexcept {
        if (this != &other) {
            Data_i::operator=(std::move(other));
            type = std::move(other.type);
            buffer_f = std::move(other.buffer_f);
            buffer_s = std::move(other.buffer_s);
            indexes_b = std::move(other.indexes_b);
            offset = other.offset;
            buffer = std::move(other.buffer);
        }
        return *this;
    }

    Data(const Data&) = delete;
    Data& operator=(const Data&) = delete;

    data_type getType() const;

    void setType (data_type type);

    const std::vector<uint8_t> &getBuffer() const;

    const std::fstream &getBufferF() const;

    int getMaxBufferLen() const;

    void setMaxBufferLen(int maxBufferLen);

    void clearBuffer();

    status getBufferStatus() const;

    std::vector<uint8_t> getData() const override;

    Data();

    Data(std::vector<uint8_t> buffer);

    Data(std::string str);


    // is is the index of the chunk
    //chunks length should be the same
    void insertChunk(int n, std::vector<uint8_t> chunk);

    void addChunk(int n, std::vector<uint8_t> chunk) override;

    void addChunk(int n, std::string chunk);

    void show() const override;

    std::vector<std::vector<uint8_t>> getDataByPackets() override;

    void addIndex(int index);

    std::vector<int> getIndexesBuffer();

    void fromChunkVec(std::vector<uint8_t> buffer);

    void fromString(std::string str);

    void forEachPacket(std::function<void(std::vector<uint8_t>, int)> func) override;

    std::string toString() const override;

    ~Data() {
        indexes_b.clear();
    }
};


#endif //PKS_PROJECT_DATA_H
