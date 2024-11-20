#include <iostream>
#include <memory>
#include <Reader/MyReader.h>
#include <chrono>
#include <stdexcept>
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

    std::cout << "Received integer: " << id << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::unique_ptr<std::istream> inputStream = std::make_unique<std::istream>(std::cin.rdbuf());
    MyReader reader(std::move(inputStream), id, "fifo" + std::to_string(id));

    std::cout << "start readingasdfasdfasdf" << std::endl;

    reader.read();


    return 0;
}