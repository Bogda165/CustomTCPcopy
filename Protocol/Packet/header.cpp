//
// Created by User on 23/10/2024.
//

#include "header.h"

Header::Header() {
    flags = std::bitset<7>();
    message_id = -1;
    packet_id = -1;
    offset = -1;
    sequence_number = -1;
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
    packet_id = 0;
    offset = 0;
    sequence_number = 0;
    check_sum = 0;

    message_id |= buffer[1] << 24;
    message_id |= buffer[2] << 16;
    message_id |= buffer[3] << 8;
    message_id |= buffer[4];

    packet_id |= buffer[5] << 24;
    packet_id |= buffer[6] << 16;
    packet_id |= buffer[7] << 8;
    packet_id |= buffer[8];

    offset |= buffer[9] << 24;
    offset |= buffer[10] << 16;
    offset |= buffer[11] << 8;
    offset |= buffer[12];

    sequence_number |= buffer[13] << 24;
    sequence_number |= buffer [14] << 16;
    sequence_number |= buffer [15] << 8;
    sequence_number |= buffer [16];

    check_sum |= buffer[17] << 24;
    check_sum |= buffer [18] << 16;
    check_sum |= buffer [19] << 8;
    check_sum |= buffer [20];

    uint8_t flags_byte = buffer[0];

    std::bitset<8> flags_bits(flags_byte);

    flags[0] = flags_bits[0];
    flags[1] = flags_bits[1];
    flags[2] = flags_bits[2];
    flags[3] = flags_bits[3];
    flags[4] = flags_bits[4];
    flags[5] = flags_bits[5];
    flags[6] = flags_bits[6];

    std::cout << "Header: " << message_id << " " << packet_id << " " << sequence_number  << " " << offset << std::endl;
}

std::vector<uint8_t> Header::toU8() {

    int val = message_id;

    std::vector<uint8_t> buffer(getLength(), 0);
    buffer[1] = (val >> 24) & 0xFF;
    buffer[2] = (val >> 16) & 0xFF;
    buffer[3] = (val >> 8) & 0xFF;
    buffer[4] = val & 0xFF;

    val = packet_id;
    buffer[5] = (val >> 24) & 0xFF;
    buffer[6] = (val >> 16) & 0xFF;
    buffer[7] = (val >> 8) & 0xFF;
    buffer[8] = val & 0xFF;

    val = offset;
    buffer[9] = (val >> 24) & 0xFF;
    buffer[10] = (val >> 16) & 0xFF;
    buffer[11] = (val >> 8) & 0xFF;
    buffer[12] = val & 0xFF;

    val = sequence_number;
    buffer[13] = (val >> 24) & 0xFF;
    buffer[14] = (val >> 16) & 0xFF;
    buffer[15] = (val >> 8) & 0xFF;
    buffer[16] = val & 0xFF;

    val = check_sum;
    buffer[17] = (val >> 24) & 0xFF;
    buffer[18] = (val >> 16) & 0xFF;
    buffer[19] = (val >> 8) & 0xFF;
    buffer[20] = val & 0xFF;

    buffer[0] = static_cast<uint8_t>(flags.to_ulong());


    std::cout << "Header in u8: ";

    for(auto i: buffer) {
        std::cout << static_cast<int>(i) << " ";
    }
    std::cout << std::endl;

    return buffer;
}

int Header::getMessageId()const {
    return message_id;
}

void Header::setMessageId(int message_id) {
    this->message_id = message_id;
}

void Header::show() const {
    std::cout << "Message id: " << message_id << "\npakcet id: " << packet_id << "\nOffset: " << offset << "\nSeq number" << sequence_number << "\nbytes: " << flags[0] << flags[1] << flags[2] << std::endl;

}

int Header::getCheckSum() const {
    return this->check_sum;
}

void Header::setCheckSum(int check_sum) {
    this->check_sum = check_sum;
}

int Header::getLength() {
    return sizeof (Header);
}

int Header::getPacketId() const {
    return packet_id;
}

void Header::setPacketId(int packet_id) {
    this->packet_id = packet_id;
}

int Header::getOffset() const {
    return offset;
}

void Header::setOffset(int _offset) {
    offset = _offset;
}

void Header::setSequenceNumber(int _sq) {
    this->sequence_number = _sq;
}

int Header::getSequenceNumber() const {
    return this->sequence_number;
}