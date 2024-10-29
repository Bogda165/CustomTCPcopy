//
// Created by User on 23/10/2024.
//

#ifndef PKS_PROJECT_HEADER_H
#define PKS_PROJECT_HEADER_H
#include <bitset>
#include <vector>
#include <iostream>
#include "../Sendable/Sendable.h"

enum class Flags {
    ACK = 0,
    SYN = 1,
};

//first byte is a flag

class Header: public Sendable {
private:
    //flags ack syn
    std::bitset<2> flags;
    int message_id;
    int packet_id;
    int offset;
    int sequence_number;
public:
    Header();

    Header(int packet_id, int message_id, int offset, int sequence_number): Header() {
        this->packet_id = packet_id;
        this->message_id = message_id;
        this->offset = offset;
        this->sequence_number = sequence_number;
    }

    Header(int packet_id, int message_id, int offset): Header() {
        this->packet_id = packet_id;
        this->message_id = message_id;
        this->offset = offset;
    }

    Header(int packet_id, int message_id): Header() {
        this->packet_id = packet_id;
        this->message_id = message_id;
    }

    template<typename... Args>
    explicit Header(int packet_id, int message_id, Args... flags) {
        this->packet_id = packet_id;
        this->message_id = message_id;
        this->offset = -1;
        this->sequence_number = -1;
        (this->setFlag(flags), ...);
    }

    template<typename... Args>
    explicit Header(Args... flags) {
        this->offset = -1;
        this->sequence_number = -1;
        this->packet_id = 0;
        this->message_id = 0;
        (this->setFlag(flags), ...);
    }

    virtual void fromU8(std::vector<uint8_t> buffer) override;

    virtual std::vector<uint8_t> toU8() override;

    bool getFlags(Flags flag);

    void setFlag(Flags flag);

    template<typename... Args>
    void setFlags(Args... flags){
        (this->setFlag(flags), ...);
    }

    int getMessageId() const;

    void setMessageId(int messageId);

    int getPacketId() const;

    void setPacketId(int packet_id);

    int getOffset() const;

    void setOffset(int _offset);

    int getSequenceNumber() const;

    void setSequenceNumber(int _sq);

    void show() const;

    static int getLength();
};




#endif //PKS_PROJECT_HEADER_H
