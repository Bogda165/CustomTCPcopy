//
// Created by User on 03/11/2024.
//

#ifndef PKS_PROJECT_READER_H
#define PKS_PROJECT_READER_H
#include <vector>
#include <memory>
#include <iostream>
#include <thread>

class Reader {
public:
    static const int read_buffer_size = 10;
protected:
    int id;
    std::unique_ptr<std::istream> input_s;
    uint8_t buffer[read_buffer_size];
    int offset = 0;
    std::string command = "/exit()";

protected:
    virtual void moveBufferImpl() = 0; //free the buffer, and offset
public:

    Reader(std::unique_ptr<std::istream> inputStream, int _id) : input_s(std::move(inputStream)), id(_id), offset(0) {
    }

    void fallBack() {
        offset = 0;
    }

    void moveBuffer() {
        moveBufferImpl();
        fallBack();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    bool readBuffer() {
        bool is_the_end = false;
        std::string line;
        if (std::getline(*input_s, line)) {
            {
                auto pos = line.find("/exit");
                //check if not the last line
                if (pos != std::string::npos) {
                    is_the_end = true;
                    line.erase(pos);
                }
            }

            line += '\n';
            std::cout << "Line read: " << line << std::endl;
            while (offset + line.length() >= read_buffer_size) {
                // add a part of the line and move the buffer
                int first_part = read_buffer_size - offset;
                std::copy(line.begin(), line.begin() + first_part, buffer + offset);
                line.erase(0, first_part);
                offset = read_buffer_size;
                moveBuffer();
            }
            std::copy(line.begin(), line.end(), buffer + offset);
            offset += line.length();
        } else {
            throw std::runtime_error("Failed to read a line!!!");
        }

        return is_the_end;
    }

    void read() {
        while(!readBuffer()) {

        }
        moveBuffer();
    }


    bool isGood() const {
        return input_s->good();
    }

    int getId() const {
        return id;
    }

    std::string getString() const {
        return std::string(reinterpret_cast<const char*>(buffer), offset);
    }

    std::vector<uint8_t> getBuffer() const {
        std::vector<uint8_t> _buffer(buffer, buffer + offset);
        return _buffer;
    }
};


#endif //PKS_PROJECT_READER_H
