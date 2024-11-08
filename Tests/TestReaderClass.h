//
// Created by User on 04/11/2024.
//

#ifndef PKS_PROJECT_TESTREADERCLASS_H
#define PKS_PROJECT_TESTREADERCLASS_H
#include <Reader/Reader.h>

class TestReaderClass: public Reader {
public:
    std::string str;
protected:
    void moveBufferImpl() override {
        std::cout << "moved line: ";
        for (unsigned char& i: buffer) {
            std::cout << i;
        }
        std::cout << std::endl;

        str.append(reinterpret_cast<char*>(buffer), offset);

        std::cout << "Totale string: " <<str << std::endl;
    }
public:
    explicit TestReaderClass(std::unique_ptr<std::istream> stream): Reader(std::move(stream)), str() {};

    void read() {
        while(!readBuffer()) {

        }
        moveBuffer();
    }
};


#endif //PKS_PROJECT_TESTREADERCLASS_H
