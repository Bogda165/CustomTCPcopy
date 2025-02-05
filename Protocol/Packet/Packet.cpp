//
// Created by User on 26/10/2024.
//

#include "Packet.h"

#include <utility>

std::vector<uint8_t> Packet::getChunk() {
    return std::move(this->chunk);
}

Header Packet::getHeader() {
    return std::move(this->header);
}

Packet::Packet() {
    header = Header();
    chunk = std::vector<uint8_t>();
}

Packet::Packet(Header _header, std::vector<uint8_t> _chunk){
    this->header = _header;
    this->chunk = std::move(_chunk);
}

void Packet::show() const {
    header.show();
    std::cout << "Buffer : ";
    for (auto c: chunk) {
        std::cout << static_cast<int>(c) << " ";
    }
    std::cout << std::endl;
}

std::vector<uint8_t> Packet::toU8() {
    std::vector<uint8_t> buffer;
    auto _header = header.toU8();
    buffer.insert(buffer.end(), _header.begin(), _header.end());
    buffer.insert(buffer.end(), chunk.begin(), chunk.end());

    return buffer;
}

void Packet::fromU8(std::vector<uint8_t> buffer) {
    std::vector<uint8_t> header_b(buffer.begin(), buffer.begin() + Header::getLength());
    header.fromU8(header_b);

    if (Header::getLength() >= buffer.size()) {
        std::cout << "There is no data in the packet" << std::endl;
        return;
    }
    chunk = std::vector(buffer.begin() + Header::getLength(), buffer.end());
}

int Packet::getSequenceNumber() const {
    return header.getSequenceNumber();
}

void Packet::setSequenceNumber(int seq_n) {
    this->header.setSequenceNumber(seq_n);
}

uint8_t Packet::cuclCheckSum() {
    int sum = 0;

    for (size_t i = 0; i < chunk.size(); i += 2) {
        uint16_t word = chunk[i]; // First byte
        if (i + 1 < chunk.size()) {
            word = (word << 8) | chunk[i + 1]; // Combine with second byte
        }
        sum += word;
        // Handle carry bits (wrap around)
        if (sum > 0xFFFF) {
            sum= (sum & 0xFFFF) + 1;
        }
    }

    auto check_sum = static_cast<uint8_t>(sum);

    return check_sum;
}

Header& Packet::getHeader2() {
    return this->header;
}