# Minimum CMake version
cmake_minimum_required(VERSION 3.29)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)

# Include directories for Protocol's Reader
include_directories(${CMAKE_SOURCE_DIR}/Protocol/Reader)

# Define the ReaderProcess executable
add_executable(ReaderProcess
        reader.cpp  # Main entry point for ReaderProcess
)

# Link ReaderProcess with the Protocol library
target_link_libraries(ReaderProcess PRIVATE Protocol)

# Set output properties for ReaderProcess
set_target_properties(ReaderProcess PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
)