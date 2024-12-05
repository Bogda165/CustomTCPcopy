//
// Created by User on 26/10/2024.
//

#ifndef PKS_PROJECT_PACKET_H
#define PKS_PROJECT_PACKET_H

#include "header.h"
#include "../Sendable/Sendable.h"

class Packet: public Sendable {
private:
    Header header;
    std::vector<uint8_t> chunk;

public:
    Header getHeader();
    Header& getHeader2();
    std::vector<uint8_t> getChunk();

    Packet();
    Packet(Header _header, std::vector<uint8_t> _chunk);

    void show() const ;

    virtual std::vector<uint8_t> toU8() override;
    virtual void fromU8(std::vector<uint8_t> buffer) override;
    virtual int getSequenceNumber()const override;
    void setSequenceNumber(int seq_n) override;

    uint8_t cuclCheckSum();

    virtual std::unique_ptr<Sendable> clone() const override {
        return std::make_unique<Packet>(*this);
    }

    static std::vector<Packet> fromBuffer(std::vector<uint8_t> buffer);

    ~Packet() = default;
};


#endif //PKS_PROJECT_PACKET_H
