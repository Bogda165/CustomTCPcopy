#include "TestReaderClass.h"

int main() {
    std::unique_ptr<std::istream> inputStream = std::make_unique<std::istream>(std::cin.rdbuf());
    TestReaderClass reader(std::move(inputStream));

    reader.read();
}