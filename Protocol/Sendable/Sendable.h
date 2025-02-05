//
// Created by User on 28/10/2024.
//

#ifndef PKS_PROJECT_SENDABLE_H
#define PKS_PROJECT_SENDABLE_H
#include <vector>

class Sendable {
public:
    virtual void fromU8(std::vector<uint8_t> buffer) = 0;

    virtual std::vector<uint8_t> toU8() = 0;

    virtual int getSequenceNumber() const = 0;
    virtual void setSequenceNumber(int seq_n) = 0;

    virtual std::unique_ptr<Sendable> clone() const = 0;

    bool operator==(const Sendable& other) const {
        return this->getSequenceNumber() == other.getSequenceNumber();
    }
    
    virtual ~Sendable() {
        
    }
};


#endif //PKS_PROJECT_SENDABLE_H
