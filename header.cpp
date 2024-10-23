//
// Created by User on 23/10/2024.
//

#include "header.h"

Header::Header() {
    flags = std::bitset<2>();
    message_id = -1;
}

bool Header::getFlags(Flags flag) {
    // I mean i hope it works like it)
    return flags[static_cast<int>(flag)];
}

void Header::setFlag(Flags flag) {
    flags.set(static_cast<int>(flag));
}

void Header::fromU8(std::vector<uint8_t> buffer) {
    message_id = 0;

    message_id |= buffer[1] << 24;
    message_id |= buffer[2] << 16;
    message_id |= buffer[3] << 8;
    message_id |= buffer[4];

    uint8_t flags_byte = buffer[0];

    std::bitset<8> flags_bits(flags_byte);

    flags[0] = flags_bits[0];
    flags[1] = flags_bits[1];
}

std::vector<uint8_t> Header::toU8() {

    int val = message_id;

    std::vector<uint8_t> buffer(5, 0);
    buffer[1] = (val >> 24) & 0xFF;
    buffer[2] = (val >> 16) & 0xFF;
    buffer[3] = (val >> 8) & 0xFF;
    buffer[4] = val & 0xFF;

    buffer[0] = static_cast<uint8_t>(flags.to_ulong());
}

int Header::getMessageId()const {
    return message_id;
}

void Header::setMessageId(int message_id) {
    this->message_id = message_id;
}

void Header::show() const {
    std::cout << "Message id: " << message_id << " bytes: " << flags[0] << flags[1] << std::endl;
}