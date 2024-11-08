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

class Data: public Data_i<std::vector<uint8_t>> {
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
    data_type getType() const;

    void setType(data_type type);

    const std::vector<uint8_t> &getBuffer() const;

    const std::fstream &getBufferF() const;

    int getMaxBufferLen() const;

    void setMaxBufferLen(int maxBufferLen);

    void clearBuffer();

    status getBufferStatus() const;

    std::vector<uint8_t> getData() override;

    Data();

    Data(std::vector<uint8_t> buffer);

    Data(std::string str);


    // is is the index of the chunk
    //chunks length should be the same
    void insertChunk(int n, std::vector<uint8_t> chunk);

    void addChunk(int n, std::vector<uint8_t> chunk) override;

    void addChunk(int n, std::string chunk);

    void show_data();

    std::string toString();

    std::vector<std::vector<uint8_t>> getDataByPackets() override;

    void addIndex(int index);

    std::vector<int> getIndexesBuffer();

    void fromChunkVec(std::vector<uint8_t> buffer);

    void fromString(std::string str);
};


#endif //PKS_PROJECT_DATA_H
