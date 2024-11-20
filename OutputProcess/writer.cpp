#include <iostream>
#include <memory>
#include <Reader/MyReader.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>

int getFifo(std::string fifo_name, int type) {
    if (mkfifo(fifo_name.c_str(), 0666) == -1) {
        perror("mkfifo");
        throw std::runtime_error("Error while creating a fifo!!!");
    }

    int in_fifo_fd = open(fifo_name.c_str(), type);
    if (in_fifo_fd == -1) {
        perror("open");
        throw std::runtime_error("Error while trying to open a fifi!!!");
    }

    return in_fifo_fd;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "No integer argument provided!" << std::endl;
        return 1;
    }

    int id;

    try {
        id = std::stoi(argv[1]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid integer argument: " << argv[1] << std::endl;
        return 1;
    } catch (const std::out_of_range& e) {
        std::cerr << "Integer argument out of range: " << argv[1] << std::endl;
        return 1;
    }

    auto fifo_id = getFifo("write_f", O_RDONLY);

    char buffer[11];
    size_t bytesRead;


    while(true) {
        bytesRead = read(fifo_id, buffer, sizeof(buffer) - 1);
        if(bytesRead > 0) {

            buffer[bytesRead] = '\0';
            std::cout << "Packet recv: " << buffer << std::endl;
        }else {
            // I mean its ub?
        }
    }

    return 0;
}