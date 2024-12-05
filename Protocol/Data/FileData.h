//
// Created by User on 15/11/2024.
//

#ifndef PKS_PROJECT_FILEDATA_H
#define PKS_PROJECT_FILEDATA_H

#include <Data/data_i.h>
#include <fstream>
#include <optional>

struct CompareFirst {
    bool operator()(const std::pair<int, std::vector<uint8_t>>& a, const std::pair<int, std::vector<uint8_t>>& b) {
        return a.first > b.first; // Min-heap based on the first element of the pair
    }
};


class FileData: public Data_i<std::priority_queue<std::pair<int, std::vector<uint8_t>>, std::vector<std::pair<int, std::vector<uint8_t>>>, CompareFirst>, std::pair<int, std::vector<uint8_t>>, int, std::vector<uint8_t>> {
    //buffer will represent packets that came out of order
private:
    std::optional<std::string> name;
    std::ofstream file;

    static std::string path;

public:
    FileData(FileData&& other) noexcept
            : name(std::move(other.name)), file(std::move(other.file)) {}

    // Move assignment operator
    FileData& operator=(FileData&& other) noexcept {
        if (this != &other) {
            name = std::move(other.name);
            file = std::move(other.file);
        }
        return *this;
    }

    // Deleted copy constructor and copy assignment operator
    FileData(const FileData&) = delete;
    FileData& operator=(const FileData&) = delete;

    FileData(): Data_i<std::priority_queue<std::pair<int, std::vector<uint8_t>>, std::vector<std::pair<int, std::vector<uint8_t>>>, CompareFirst>, std::pair<int, std::vector<uint8_t>>, int, std::vector<uint8_t>>() {
        name = "unlabeled";
    }

    void setName(const std::string fileName) {
        if (file.is_open()) {
            file.close();
            std::rename(name->c_str(), fileName.c_str());
            file.open(fileName, std::ios::app | std::ios::binary);
        }
        name = fileName;
    }

    void openFile() {
        if (name.has_value()) {
            file.open(path + name.value(), std::ios::out | std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("Could not open file: " + path + name.value());
            }
        } else {
            throw std::runtime_error("File name is not set.");
        }
    }

    void closeFile() {
        if (file.is_open()) {
            file.close();
        }
    }

    virtual std::vector<uint8_t> getData() const override {
        throw std::runtime_error("old method should not be used!!!");
    }

    virtual void addChunk(int n, std::vector<uint8_t> chunk) override {
        if(chunk_len == 0 || chunk_len < chunk.size()) {
            chunk_len = chunk.size();
        }
        if (n == 0) {
            setName(std::string(chunk.begin(), chunk.end()));
            return;
        }
        data_amount ++;

        if (!file.is_open()) {
            openFile();
        }

        //do stuff with file
        //if packet came out of order
        if (data_len < n - 1) {
            buffer.push(std::make_pair(n, std::move(chunk)));
        }else {
            data_len ++;
            file.write(reinterpret_cast<const char*>(chunk.data()), chunk.size());
            file.flush();
        }
    }
    virtual std::vector<std::vector<uint8_t>> getDataByPackets() override {
        std::ifstream _file;
        if (file.is_open()) {
            file.close();

            _file.open(path + name.value(), std::ios::in | std::ios::binary);
            if (!_file.is_open()) {
                throw std::runtime_error("Could not open file: " + path + name.value());
            }
        }

        std::vector<std::vector<uint8_t>> _result;

        while (true) {
            std::vector<uint8_t> chunk(chunk_len);
            _file.read(reinterpret_cast<char*>(chunk.data()), chunk.size());

            if (_file.gcount() <= 0) {
                break;
            }

            chunk.resize(_file.gcount());
            _result.push_back(std::move(chunk));
        }

        return std::move(_result);
    }


    void show() const override {
        std::cout << path << "/";
        if (name.has_value()) {
            std::cout << path << name.value();
        }
        std::cout << std::endl;
    }

    void forEachPacket(std::function<void(std::vector<uint8_t>, int)> func) override {
        auto packets = this->getDataByPackets();
        int packet_id = 0;
        for (auto packet: packets) {
            func(std::move(packet), packet_id);
            packet_id ++;
        }
    }

    std::string getName() const {
        return path + (name.has_value()? name.value(): " no name");
    }

    std::string toString() const override {
        std::string _return;
        _return += path + "/";
        if (name.has_value()) {
            _return += path + name.value() ;
        }
        return _return;
    }

    ~FileData() {
        closeFile();
    }
};


#endif //PKS_PROJECT_FILEDATA_H
