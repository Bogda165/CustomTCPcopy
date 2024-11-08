#ifndef PKS_PROJECT_MYREADER_H
#define PKS_PROJECT_MYREADER_H
#include "Reader.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <chrono>

class MyReader: public Reader {
private:
    int out_fifo_fd;
protected:
    void moveBufferImpl() override {
        std::cout << "Written to the pipe: " << getString() << std::endl;
        write(out_fifo_fd, getString().c_str(), getString().size());
    }
public:
    MyReader(std::unique_ptr<std::istream> inputStream, int _id, const std::string& name)
            : Reader(std::move(inputStream), _id) {
        std::string fifoPath = "fifo" + std::to_string(_id);

        std::cout << "chlen" << std::endl;
        out_fifo_fd = open(fifoPath.c_str(), O_WRONLY);
        if (out_fifo_fd == -1) {
            perror("open");
            throw std::runtime_error("Failed to open FIFO for writing");
        }
    }

    ~MyReader() {
        write(out_fifo_fd, "/e", 2);
        close(out_fifo_fd);

        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "Trying to close: " << ("fifo" + std::to_string(id)).c_str() << std::endl;
        if (unlink(("fifo" + std::to_string(id)).c_str()) == -1) {
            std::cerr << errno << std::endl;
        }
        system("exit");
    }

    int getFifoId() const {
        return out_fifo_fd;
    }
};

#endif //PKS_PROJECT_MYREADER_H