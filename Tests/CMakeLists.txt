# my_project/tests/CMakeLists.txt
enable_testing()

# Find Google Test package
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

include_directories(${CMAKE_SOURCE_DIR}/src)

# Add the test executable
add_executable(test_reader test_reader.cpp ../Protocol/Reader/Reader.cpp
        TestReaderClass.cpp
        TestReaderClass.h
        main.cpp)

# Link Google Test and pthread
target_link_libraries(test_reader ${GTEST_LIBRARIES} pthread)

add_library(Reader ../Protocol/Reader/Reader.cpp)

# Add a test
add_test(NAME ReaderTest COMMAND test_reader)
