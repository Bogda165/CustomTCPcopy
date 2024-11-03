# my_project/tests/CMakeLists.txt
enable_testing()

# Find Google Test package
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add the test executable
add_executable(test_reader test_reader.cpp ../src/Reader/Reader.cpp)

# Link Google Test and pthread
target_link_libraries(test_reader ${GTEST_LIBRARIES} pthread)

# Add a test
add_test(NAME ReaderTest COMMAND test_reader)