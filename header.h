//
// Created by User on 23/10/2024.
//

#ifndef PKS_PROJECT_HEADER_H
#define PKS_PROJECT_HEADER_H
#include <bitset>
#include <vector>
#include <iostream>

enum class Flags {
    ACK = 0,
    SYN = 1,
};

//first byte is a flag

class Header {
private:
    //flags ack syn
    std::bitset<2> flags;
    int message_id;
public:
    Header();

    template<typename... Args>
    Header(int message_id, Args... flags) {
        this->message_id = message_id;
        (this->setFlag(flags), ...);
    }

    void fromU8(std::vector<uint8_t> buffer);

    std::vector<uint8_t> toU8();

    bool getFlags(Flags flag);

    void setFlag(Flags flag);

    template<typename... Args>
    void setFlags(Args... flags){
        (this->setFlag(flags), ...);
    }

    int getMessageId() const;

    void setMessageId(int messageId);

    void show() const;
};




#endif //PKS_PROJECT_HEADER_H
