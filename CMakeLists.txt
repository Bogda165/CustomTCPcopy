cmake_minimum_required(VERSION 3.29)
project(Pks_project)

set(CMAKE_CXX_STANDARD 20)

# Set the output directories for executables and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add the directory where yaml-cpp is installed
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/yaml-cpp/0.8.0")

# Link Boost if needed by Protocol
find_package(Boost 1.70 REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif()

# Find yaml-cpp
find_package(yaml-cpp REQUIRED)
if(NOT yaml-cpp_FOUND)
    message(FATAL_ERROR "yaml-cpp not found!")
endif()

# Include directories for Boost and Protocol
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/Protocol)
include_directories(${CMAKE_SOURCE_DIR}/Protocol/Socket)  # Add this line
include_directories(${CMAKE_SOURCE_DIR}/src)

# Link directories for Homebrew libraries
link_directories(/opt/homebrew/lib)

# Include subdirectories for Protocol, ReaderProcess, OutputProcess, and Application
add_subdirectory(Protocol)
add_subdirectory(ReaderProcess)
add_subdirectory(OutputProcess)

# Main executable
add_executable(main src/main.cpp
        src/Configure.cpp
        src/Configure.h)

# Ensure the Protocol and Application are built before linking
target_link_libraries(main PRIVATE Protocol yaml-cpp Boost::system)

add_dependencies(main ReaderProcess OutputProcess)