cmake_minimum_required(VERSION 3.29)
project(Pks_project)

set(CMAKE_CXX_STANDARD 20)

# Set the output directories for executables and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Link Boost if needed by Protocol
find_package(Boost 1.70 REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif()

# Include Boost headers and other needed directories
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/Protocol)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Include subdirectories for Protocol and ReaderProcess
add_subdirectory(Protocol)
add_subdirectory(ReaderProcess)
add_subdirectory(OutputProcess)

# Main executable
add_executable(main src/main.cpp)  # Correctly define the main target

# Ensure that Protocol is built before linking
target_link_libraries(main PRIVATE Protocol)   # Link the main target with Protocol

add_dependencies(main ReaderProcess OutputProcess)
