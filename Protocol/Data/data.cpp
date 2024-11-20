//
// Created by User on 23/10/2024.
//

#include "data.h"


Data::Data() {
    type = data_type::VEC;
    buffer = std::vector<uint8_t>(max_buffer_len, 0);
    offset = -1;
    data_len = 0;
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

data_type Data::getType() const {
    return type;
}

const std::vector<uint8_t> &Data::getBuffer() const {
    return buffer;
}

const std::fstream &Data::getBufferF() const {
    return buffer_f;
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

    if (n >= data_len) {
        data_len = n + 1;
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

    if (n >= data_len) {
        data_len = n + 1;
        offset = chunk_len - chunk.size();
    }

    std::copy(chunk.begin(), chunk.end(), _buffer.begin());

    this->insertChunk(n, _buffer);
}

std::vector<uint8_t> Data::getData() const{
    if (type == data_type::VEC) {
        auto _vec = std::vector<uint8_t>();

        _vec.assign(buffer.begin(), buffer.begin() + chunk_len * (data_len) - offset);

        return _vec;
    }else{
        //realise for a file
    }
}

std::vector<std::vector<uint8_t>> Data::getDataByPackets() {
    if (type == data_type::VEC) {
        auto _vec = std::vector<std::vector<uint8_t>>();

        for(int packet_i = 0; packet_i < data_len - 1; packet_i ++) {
            _vec.push_back(std::move(std::vector(buffer.begin() + packet_i * chunk_len, buffer.begin() + (packet_i + 1) * chunk_len)));
        }

        _vec.push_back(std::move(std::vector(buffer.begin() + (data_len - 1) * chunk_len, buffer.begin() + chunk_len * data_len - offset)));

        return _vec;
    }else{
        //realise for a file
    }
}


void Data::show() const {
    for (auto i: this->getData()) {
        std::cout << static_cast<int>(i) << " ";
    }

    std::cout << std::endl;
}

Data::Data(std::string str): Data() {

    int n = str.length() / this->getChunkLen();

    for (int i = 0; i < n; i++) {
        std::string _str = str.substr(i * this->getChunkLen(), this->getChunkLen());
        std::cout << _str << ":> " <<_str.length() << std::endl;
        this->addChunk(i, _str);
    }
    std::string _str = str.substr(n * this->getChunkLen(), str.length() % this->getChunkLen());
    std::cout << _str << ":> " <<_str.length() << std::endl;
    this->addChunk(n, _str);
}

void Data::fromString(std::string str){

    int n = str.length() / this->getChunkLen();

    for (int i = 0; i < n; i++) {
        std::string _str = str.substr(i * this->getChunkLen(), this->getChunkLen());
        std::cout << _str << ":> " <<_str.length() << std::endl;
        this->addChunk(i, _str);
    }
    std::string _str = str.substr(n * this->getChunkLen(), str.length() % this->getChunkLen());
    std::cout << _str << ":> " <<_str.length() << std::endl;
    this->addChunk(n, _str);
}

std::string Data::toString() {
    auto _buffer = this->getData();
    std::string str(_buffer.begin(), _buffer.end());

    return str;
}

Data::Data(std::vector<uint8_t> buffer): Data() {
    int n = buffer.size() / this->getChunkLen();

    for (int i = 0; i < n; i++) {
        this->addChunk(i, std::move(std::vector(buffer.begin() + i * this->getChunkLen(), buffer.begin() + (i + 1) * this->getChunkLen())));
    }
    this->addChunk(n, std::move(std::vector(buffer.begin() + n * this->getChunkLen(), buffer.end())));
}

void Data::fromChunkVec(std::vector<uint8_t> buffer) {
    int n = buffer.size() / this->getChunkLen();

    for (int i = 0; i < n; i++) {
        this->addChunk(i, std::move(std::vector(buffer.begin() + i * this->getChunkLen(), buffer.begin() + (i + 1) * this->getChunkLen())));
    }
    this->addChunk(n, std::move(std::vector(buffer.begin() + n * this->getChunkLen(), buffer.end())));
}


void Data::addIndex(int index) {
    indexes_b.push_back(index);
}

std::vector<int> Data::getIndexesBuffer() {
    return std::move(this->indexes_b);
}

void Data::forEachPacket(std::function<void(std::vector<uint8_t>, int)> func) {
    auto packets = this->getDataByPackets();
    int packet_id = 0;
    for (auto packet: packets) {
        func(std::move(packet), packet_id);
        packet_id ++;
    }
}