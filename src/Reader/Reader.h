//
// Created by User on 03/11/2024.
//

#ifndef PKS_PROJECT_READER_H
#define PKS_PROJECT_READER_H
#include <vector>
#include <memory>
#include <iostream>

class Reader {
private:
    std::unique_ptr<std::istream> input_s;
public:

    Reader(std::unique_ptr<std::istream> inputStream) : input_s(std::move(inputStream)) {}

    std::string readLine() {
        std::string line;
        if (std::getline(*input_s, line)) {
            return line;
        }
        return "";
    }


    bool isGood() const {
        return input_s->good();
    }

};


#endif //PKS_PROJECT_READER_H
