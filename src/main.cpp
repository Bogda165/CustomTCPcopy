#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <chrono>
#include <Data/data.h>
#include <Socket/MySocket.h>
#include <Receivers/myReceiver.h>
#include "Data/data_i.h"
#include <Data/DinamicData.h>
#include <atomic>


int Data::max_buffer_len = 1024;

std::string FileData::path = "";

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

    auto recv_thread = receiver->run(1);

    std::string cmd;
    int message_id = 0;
    std::string arg;
    std::string data;
    auto seq_n = std::make_shared<std::atomic<int>> (-1);
    std::function<void(std::vector<uint8_t>, int, Header)> lambda = [&](std::vector<uint8_t> packet, int packet_id, Header pre_header) {
        seq_n->fetch_add(1, std::memory_order_acquire);

        std::cout << seq_n->load() << ": ";
        for (auto i: packet) {
            std::cout << static_cast<int>(i) << " ";
        }
        std::cout << std::endl;

        pre_header.setPacketId(packet_id);
        pre_header.setOffset(packet.size());
        pre_header.setSequenceNumber(seq_n->load());

        Packet to_send(pre_header, packet);

        //to_send.show();

        std::unique_ptr<Sendable> obj = std::make_unique<Packet>(to_send);

        socket->addToContainer(std::move(obj));
    };

    std::function<void(int, int)> send_ending_packet = [&](int message_id, int amount_of_packets) {
        Header ending_header(Flags::END);
        ending_header.setMessageId(message_id);
        ending_header.setSequenceNumber(-1);
        ending_header.setPacketId(amount_of_packets);

        std::unique_ptr<Sendable> sendable = std::make_unique<Header>(ending_header);

        socket->addToContainer(std::move(sendable));
    };

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
            Header pre_header;
            pre_header.setMessageId(message_id);
            if (arg == "-np") {
                auto data_m = std::make_shared<std::mutex>(); // Initialize mutex with a valid object
                auto packet_data = std::make_shared<DinamicData>();
                auto flag = std::atomic_bool(true);

                // Create a new process
                create_runner_script("Terminal", "/Users/user/CLionProjects/Pks_project/cmake-build-debug/bin", message_id);
                auto fifo_id = getFifo("fifo" + std::to_string(message_id), O_RDONLY);

                // Pass shared_ptr by value to ensure its lifetime is maintained within the thread
                auto np_thread = std::thread([data_m, packet_data, &flag, fifo_id]() {
                    char buffer[Data::getChunkLen() + 1];
                    size_t bytesRead;
                    int packet_id = 0;

                    while (true) {
                        bytesRead = read(fifo_id, buffer, sizeof(buffer) - 1);
                        if (bytesRead > 0) {
                            if (bytesRead > 1 && buffer[0] == '/' && buffer[1] == 'e') {
                                break;
                            }
                            buffer[bytesRead] = '\0';
                            std::cout << "Parent received: " << buffer << std::endl;
                            {
                                std::lock_guard<std::mutex> lock(*data_m);  // Corrected lock access
                                packet_data->addChunk(packet_id, buffer);
                            }
                            packet_id++;
                        } else {
                            // Optional: sleep to prevent busy-waiting
                            // std::this_thread::sleep_for(std::chrono::seconds(1));
                        }
                    }

                    close(fifo_id);
                    flag.store(false, std::memory_order_release);
                });

                np_thread.detach();

                auto np_send_thread = std::thread([data_m, &flag, packet_data, lambda, pre_header, send_ending_packet]() {
                    while(flag.load(std::memory_order::seq_cst)) {
                        {
                            std::cout << "locking a data" << std::endl;
                            std::lock_guard<std::mutex> lock(*data_m);
                            packet_data->forEachPacketWithArgs(lambda, pre_header);
                        }
                        std::this_thread::sleep_for(std::chrono::seconds(10));
                    }

                    std::cout << "locking a data" << std::endl;
                    std::lock_guard<std::mutex> lock(*data_m);
                    packet_data->forEachPacketWithArgs(lambda, pre_header);
                    send_ending_packet(pre_header.getMessageId(), packet_data->dataAmount());
                });

                np_send_thread.detach();
            }else if (arg == "-cp"){
                Data packet_data;
                std::cout << "Enter your message : \n";
                // work in current process.
                std::string message;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                getline(std::cin, message);
                packet_data.fromString(message);

                //Add to sender
                packet_data.forEachPacketWithArgs(lambda, pre_header);
                send_ending_packet(message_id, packet_data.dataAmount());
            }else {
                std::cout << "unknown argument" << std::endl;
                continue;
            }
            message_id ++;
        }else if (cmd == "file") {
            if (arg == "-cp") {
                std::string _path = "/Users/user/CLionProjects/Pks_project/1.zip";
                auto data_m = std::make_shared<std::mutex>(); // Initialize mutex with a valid object
                auto packet_data = std::make_shared<DinamicData>();

                Header pre_header = Header(Flags::FILE);
                pre_header.setMessageId(message_id);

                auto flag = std::atomic_bool(true);


                auto np_thread = std::thread([data_m, packet_data, &flag, _path]() {
                    auto chunk_size = packet_data->getChunkLen();
                    int packet_id = 0;
                    std::string file_name;

                    //std::cin >> file_name;

                    file_name = "/Users/user/CLionProjects/Pks_project/1.zip";
                    std::cout << "Read from a file" << file_name << std::endl;

                    //TODO delete
                    //file_name = _path;

                    std::ifstream file(file_name, std::ios::binary);

                    if (!file.is_open()) {
                        throw std::runtime_error("Couldnt open a file with name " + file_name);
                    }
                    std::cout << "file was opened";

                    {
                        std::lock_guard<std::mutex> lock(*data_m);
                        {
                            auto sl_pos = file_name.rfind('/');
                            file_name = file_name.substr(sl_pos + 1, file_name.size() - sl_pos);
                            std::cout << "Name of the file: " << file_name << std::endl;
                        }
                        packet_data->addChunk(packet_id, file_name);
                        packet_id++;
                    }

                    while (true) {
                        std::vector<char> buffer(chunk_size);

                        file.read(buffer.data(), chunk_size);
                        std::streamsize bytes_read = file.gcount();

                        if (bytes_read > 0) { // Only process valid chunks
                            buffer.resize(bytes_read);

                            {
                                std::lock_guard<std::mutex> lock(*data_m);
                                std::vector<uint8_t> _buffer(buffer.begin(), buffer.end());
                                packet_data->addChunk(packet_id, _buffer);
                                std::cout << "Read: " << buffer.data() << std::endl;
                                packet_id++;
                            }
                        }

                        if (file.eof()) {
                            break; // Exit the loop when EOF is reached
                        } else if (file.fail() && !file.eof()) {
                            std::cerr << "Error reading file!" << std::endl;
                            break; // Exit on other errors
                        }
                    }
                    flag.store(false, std::memory_order_seq_cst);
                });

                np_thread.detach();

                auto np_send_thread = std::thread([data_m, &flag, packet_data, lambda, send_ending_packet, pre_header]() {
                    while (flag.load(std::memory_order::seq_cst)) {
                        {
                            std::lock_guard<std::mutex> lock(*data_m);
                            packet_data->forEachPacketWithArgs(lambda, pre_header);
                        }
                        std::this_thread::sleep_for(std::chrono::seconds(10));
                    }

                    std::cout << "locking a data" << std::endl;
                    std::lock_guard<std::mutex> lock(*data_m);
                    packet_data->forEachPacketWithArgs(lambda, pre_header);

                    // minus one, becuase of first name packet
                    send_ending_packet(pre_header.getMessageId(), packet_data->dataAmount() - 1);
                });
                np_send_thread.detach();
            }
        }else if (cmd == "show"){
            auto socket_m = socket->getMessages();
            std::lock_guard<std::mutex> lock(*socket_m.second);
            for (auto& message: *(socket_m.first)) {
                std::cout << "Message " << message.first << " ";
                auto& _data = message.second.second;
                std::visit([](auto&_data){
                        std::cout << "HUI" << std::endl;
                        using T = std::decay_t<decltype(_data)>;
                        if constexpr (std::is_same_v<T, Data>) {
                            std::cout << _data.toString();
                        } else if constexpr (std::is_same_v<T, FileData>){
                            std::cout << _data.getName();
                        }
                    }, _data);
                std::cout << std::endl;
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