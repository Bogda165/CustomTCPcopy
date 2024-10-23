//
// Created by User on 23/10/2024.
//

#include "data.h"


Data::Data() {
    type = data_type::VEC;
    max_buffer_len = 1024;
    buffer = std::vector<uint8_t>(1024, 0);
    chunk_len = 8;
    offset = 0;
}

// not save
void Data::insertChunk(int n, std::vector<uint8_t> chunk) {

    buffer_s = status::FILLED;

    if (type == data_type::VEC) {
        if (chunk.size() != chunk_len) {
            throw std::runtime_error("the length of chunk is incorrect!!!]\n\t " + std::to_string(n) + "!=" +
                                     std::to_string(chunk.size()));
        }
        std::copy(chunk.begin(), chunk.end(), buffer.begin() + chunk.size() * n);
    }else {
        // implement for file
    }
}

void Data::show() const {
    if (type == data_type::VEC) {
        for (auto i: buffer) {
            std::cout << static_cast<int>(i) << " ";
        }
        std::cout << std::endl;
    }else {
        //implement for file
    }
}

int Data::getDataLength() const {
    return data_len * chunk_len;
}

data_type Data::getType() const {
    return type;
}

const std::vector<uint8_t> &Data::getBuffer() const {
    return buffer;
}

const std::fstream &Data::getBufferF() const {
    return buffer_f;
}

int Data::getChunkLen() const {
    return chunk_len;
}

void Data::setChunkLen(int chunkLen) {
    if (buffer_s == status::FILLED) {
        throw std::runtime_error("Trying to change value, while buffer is filled, clear the buffer first");
    }

    chunk_len = chunkLen;
}

int Data::getMaxBufferLen() const {
    return max_buffer_len;
}

void Data::setMaxBufferLen(int maxBufferLen) {
    if (buffer_s == status::FILLED) {
        throw std::runtime_error("Trying to change value, while buffer is filled, clear the buffer first");
    }

    max_buffer_len = maxBufferLen;
    buffer = std::vector<uint8_t>(maxBufferLen, 0);
}

status Data::getBufferStatus() const {
    return buffer_s;
}

void Data::clearBuffer() {
    if (type == data_type::VEC) {
        buffer.clear();
        buffer = std::vector<uint8_t>(1024, 0);
    }else {
        //clear file
    }

    type = data_type::VEC;
    buffer_s = status::EMPTY;
}


void Data::addChunk(int n, std::vector<uint8_t> chunk) {
    auto _buffer = std::vector<uint8_t>(chunk_len, 0);

    if (n >= max_buffer_len && type == data_type::VEC) {
        //change_to_file
    }

    if (n > data_len) {
        data_len = n;
        offset = chunk_len - chunk.size();
    }

    std::copy(chunk.begin(), chunk.end(), _buffer.begin());

    this->insertChunk(n, std::move(_buffer));
}

void Data::addChunk(int n, std::string chunk) {
    auto _buffer = std::vector<uint8_t>(chunk_len, 0);

    if (n >= max_buffer_len && type == data_type::VEC) {
        //change_to_file
    }

    if (n > data_len) {
        data_len = n;
        offset = chunk_len - chunk.size();
    }

    std::copy(chunk.begin(), chunk.end(), _buffer.begin());

    this->insertChunk(n, _buffer);
}

std::vector<uint8_t> Data::getData(){
    if (type == data_type::VEC) {
        auto _vec = std::vector<uint8_t>();

        _vec.assign(buffer.begin(), buffer.begin() + chunk_len * (data_len + 1) - offset);

        return _vec;
    }else{
        //realise for a file
    }
}

void Data::show_data() {
    for (auto i: this->getData()) {
        std::cout << static_cast<int>(i) << " ";
    }

    std::cout << std::endl;
}

void fromString(std::string& str, Data& data) {

    int n = str.length() / data.getChunkLen();

    for (int i = 0; i < n; i++) {
        std::string _str = str.substr(i * data.getChunkLen(), data.getChunkLen());
        std::cout << _str << ":> " <<_str.length() << std::endl;
        data.addChunk(i, _str);
    }
    std::string _str = str.substr(n * data.getChunkLen(), str.length() % data.getChunkLen());
    std::cout << _str << ":> " <<_str.length() << std::endl;
    data.addChunk(n, _str);
}

std::string Data::toString() {
    auto _buffer = this->getData();
    std::string str(_buffer.begin(), _buffer.end());

    return str;
}
