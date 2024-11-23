//
// Created by User on 09/11/2024.
//

#ifndef PKS_PROJECT_DINAMICDATA_H
#define PKS_PROJECT_DINAMICDATA_H

#include <Data/data.h>
#include <unordered_map>
//
class DinamicData: public Data_i<std::unordered_map<int, std::vector<uint8_t>>, std::pair<int, std::vector<uint8_t>>> {
private:

public:
    DinamicData() = default;


    std::vector<uint8_t> getData() const override {
        throw std::runtime_error("Using stupid method, do not do it plz man");
    }
    void addChunk(int n, std::vector<uint8_t> chunk) override {
        data_amount ++;
        if(chunk.size() != chunk_len) {
            std::cout << "Error different chunk size and chunk len " << chunk_len << chunk.size() << std::endl;
        }
        buffer[n] = std::move(chunk);
        if (n > data_len) {
            data_len = n;
        }
    }

    void addChunk(int n, std::string chunk) {
        if(chunk_len == 0 || chunk_len < chunk.size()) {
            chunk_len = chunk.size();
        }
        data_amount ++;
        buffer[n] = std::move(std::vector<uint8_t>(chunk.begin(), chunk.end()));
        if (n > data_len) {
            data_len = n;
        }
    }
    std::vector<std::pair<int, std::vector<uint8_t>>> getDataByPackets() override {
        auto _vec = std::vector<std::pair<int, std::vector<uint8_t>>>();

        for(int i = 0; i <= data_len; i++) {
            if (buffer.find(i) != buffer.end()) {
                if (buffer[i].size() != chunk_len) {
                    std::cout << "the length of chunk is incorrect!!!]\n\t " + std::to_string(chunk_len) + "!=" +
                                 std::to_string(buffer[i].size());
//                throw std::runtime_error("the length of chunk is incorrect!!!]\n\t " + std::to_string(chunk_len) + "!=" +
//                                         std::to_string(packet.second.size()));
                }

                _vec.push_back(std::make_pair(i, buffer[i]));
            }
        }

        buffer.clear();
        data_len = 0;

        return std::move(_vec);
    }



    void fromString(std::string str){

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

    void forEachPacket(std::function<void(std::vector<uint8_t>, int)> func) {
        auto packets = this->getDataByPackets();
        for (auto packet: packets) {
            func(std::move(packet.second), packet.first);
        }
    }

    void show() const override {
        for (auto packet:buffer) {
            std::cout << packet.first << "-> ";
            for (auto i: packet.second) {
                std::cout << static_cast<int>(i) << " ";
            }
        }
    }

    std::string toString()const override {
        throw std::runtime_error("its impossible to convert dimanic data to string(");
    }

    ~DinamicData() = default;
};


#endif //PKS_PROJECT_DINAMICDATA_H
