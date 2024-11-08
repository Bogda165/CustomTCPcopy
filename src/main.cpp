#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <chrono>
#include <Data/data.h>
#include <Socket/MySocket.h>
#include <Receivers/myReceiver.h>
#include "Data/data_i.h"


int Data::max_buffer_len = 1024;
template<> int Data_i<std::vector<unsigned char>>::chunk_len = 10;

using boost::asio::ip::udp;

void create_runner_script(std::string terminal_name, std::string absolute_path, int id) {
    std::string full_file_name = "runner_script" + std::to_string(id) + ".sh";

    std::string touch_cmd = "touch runner_script" + std::to_string(id) + ".sh\n chmod +x " + full_file_name;

    std::cout << touch_cmd << std::endl;
    system(touch_cmd.c_str());

    std::ofstream file(full_file_name);
    file << "#/bin/bash\ncd " << absolute_path << "&& ./ReaderProcess " << std::to_string(id);

    std::string print_insides = "cat " + full_file_name;

    std::string run_in_terminal_cmd = "open -a " + terminal_name + " " + full_file_name;

    system(run_in_terminal_cmd.c_str());
}

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

int main() {
    boost::asio::io_context io_context;
    int _port;

    std::cout << "Enter port: ";
    std::cin >> _port;

    auto socket= std::make_shared<MySocket>(_port, "127.0.0.1", std::ref(io_context));

    auto receiver = std::make_shared<MyReceiver>(socket);

    auto recv_thread = receiver->run(2);

    std::string cmd;
    int message_id = 0;
    std::string arg;
    std::string data;
    auto seq_n = std::make_shared<std::atomic<int>> (-1);
    std::function<void(std::vector<uint8_t> packet, int packet_id)> lambda;
    //creating a lambda
    {
        lambda = [&](std::vector<uint8_t> packet, int packet_id) {

            seq_n->fetch_add(1, std::memory_order_acquire);

            //none of bellow can move upper ^
            std::cout << seq_n->load() << ": ";
            for (auto i: packet) {
                std::cout << static_cast<int>(i) << " ";
            }
            std::cout << std::endl;

            Header header;
            header.setPacketId(packet_id);
            header.setMessageId(message_id);
            header.setOffset(packet.size());
            header.setSequenceNumber(seq_n->load());
            Packet to_send(header, packet);

            to_send.show();

            std::unique_ptr<Sendable> obj = std::make_unique<Packet>(to_send);

            socket->addToContainer(std::move(obj));
        };
    }


    while(cmd != "exit") {
        std::cin >> cmd >> arg;
        if (cmd == "connect") {
            //connect
            try {
                _port = std::stoi(arg);
            } catch (...) {
                std::cout << "Error while reading a port" << std::endl;
            }
            // get header
            socket->getHandShakeStat().first->tryConnect(_port);

        }else if (cmd == "message") {
            Data packet_data;
            if (arg == "-np") {
                //create a new process
                create_runner_script("Terminal", "/Users/user/CLionProjects/Pks_project/cmake-build-debug/bin", message_id);

                auto fifo_id = getFifo("fifo" + std::to_string(message_id), O_RDONLY);

                char buffer[Data::getChunkLen() + 1];
                size_t bytesRead;
                int packet_id = 0;
                while (true) {
                    bytesRead = read(fifo_id, buffer, sizeof(buffer) - 1);
                    if (bytesRead > 0) {
                        if(bytesRead > 1 && buffer[0] == '/' && buffer[1] == 'e') { break;}
                        buffer[bytesRead] = '\0';
                        std::cout << "Parent received: " << buffer << std::endl;
                        packet_data.addChunk(packet_id, buffer);
                    } else {
                        //std::this_thread::sleep_for(std::chrono::seconds(1)); // Optional: sleep to prevent busy-waiting
                    }
                    packet_id ++;
                }

                close(fifo_id);

                packet_data.forEachPacket(lambda);
            }else if (arg == "-cp"){
                std::cout << "Enter your message : \n";
                // work in current process.
                std::string message;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                getline(std::cin, message);
                packet_data.fromString(message);

                //Add to sender
                packet_data.forEachPacket(lambda);
            }else {
                std::cout << "unknown argument" << std::endl;
                continue;
            }
            message_id ++;
        }else if (cmd == "show"){
            auto socket_m = socket->getMessages();
            std::lock_guard<std::mutex> lock(*socket_m.second);
            for (auto& message: *(socket_m.first)) {
                std::cout << "Message " << message.first << " ";
                std::cout << message.second.second.toString() << std::endl;
            }
        }else {

        }
    }
/*
    create_runner_script("warp", "/Users/user/CLionProjects/Pks_project/cmake-build-debug/bin", 0);

    const char* fifoPath = "my_fifo";

    // Create a named pipe (FIFO)
    if (mkfifo(fifoPath, 0666) == -1) {
        perror("mkfifo");
        return 1;
    }

    int in_fifo_fd = open(fifoPath, O_RDONLY);
    if (in_fifo_fd == -1) {
        perror("open");
        return 1;
    }

    char buffer[1024];
    ssize_t bytesRead;
    while (true) {
        bytesRead = read(in_fifo_fd, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "Parent received: " << buffer << std::endl;
        } else {
            //std::this_thread::sleep_for(std::chrono::seconds(1)); // Optional: sleep to prevent busy-waiting
        }
    }

    close(in_fifo_fd);
*/
    return 0;
}