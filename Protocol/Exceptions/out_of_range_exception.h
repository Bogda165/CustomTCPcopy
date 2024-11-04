//
// Created by User on 02/11/2024.
//

#ifndef PKS_PROJECT_OUT_OF_RANGE_EXCEPTION_H
#define PKS_PROJECT_OUT_OF_RANGE_EXCEPTION_H
#include <exception>
#include <string>
enum class OUT_OF_RANGE{
    LEFT,
    RIGHT,
    IN,
};

class out_of_range_exc : public std::exception {
    OUT_OF_RANGE code;
    std::string message;

public:
    out_of_range_exc(OUT_OF_RANGE code, const std::string message)
            : code(code), message(message) {}

    const char* what() const noexcept override {
        return message.c_str();
    }

    OUT_OF_RANGE getCode() const {
        return code;
    }
};


#endif //PKS_PROJECT_OUT_OF_RANGE_EXCEPTION_H
