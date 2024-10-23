//
// Created by User on 23/10/2024.
//

#ifndef PKS_PROJECT_DATA_H
#define PKS_PROJECT_DATA_H
#include <vector>
#include <fstream>
#include <iostream>

enum class data_type {
    VEC,
    FILE,
};

enum class status {
    EMPTY,
    FILLED,
};

class Data {
private:
    data_type type;
    //vector of data
    std::vector<uint8_t> buffer;
    // file of data
    std::fstream buffer_f;
    //buffer status
    status buffer_s;
    //the length of the data, each time something added to a buffer, check if data_len greather then n, change it
    int data_len;

    int chunk_len;

    int max_buffer_len;

    //offset for the last buffer from the rigth side
    int offset;
public:
    data_type getType() const;

    void setType(data_type type);

    const std::vector<uint8_t> &getBuffer() const;

    const std::fstream &getBufferF() const;

    int getChunkLen() const;

    void setChunkLen(int chunkLen);

    int getMaxBufferLen() const;

    void setMaxBufferLen(int maxBufferLen);

    void clearBuffer();

    status getBufferStatus() const;

    int getDataLength() const;

    std::vector<uint8_t> getData();

    Data();


    // is is the index of the chunk
    //chunks length should be the same
    void insertChunk(int n, std::vector<uint8_t> chunk);

    void addChunk(int n, std::vector<uint8_t> chunk);

    void addChunk(int n, std::string chunk);

    void show() const;

    void show_data();

    std::string toString();
};

void fromString(std::string& str, Data& data);

#endif //PKS_PROJECT_DATA_H
