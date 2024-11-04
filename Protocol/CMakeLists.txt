cmake_minimum_required(VERSION 3.29)
project(Pks_project)

enable_testing()

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.70 REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif()

include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_executable(Pks_project src/main.cpp
        src/Socket/MySocket.cpp
        src/Socket/MySocket.h
        src/Data/data.cpp
        src/Data/data.h
        src/Socket/header.cpp
        src/Socket/header.h
        src/Receivers/receiver.cpp
        src/Receivers/receiver.h
        src/Receivers/myReceiver.cpp
        src/Receivers/myReceiver.h
        src/Socket/HandShakeStats.cpp
        src/Socket/HandShakeStats.h
        src/Packet/Packet.cpp
        src/Packet/Packet.h
        src/WindowF/WindowF.cpp
        src/WindowF/WindowF.h
        src/WindowF/MyWindow.cpp
        src/WindowF/MyWindow.h
        src/Sendable/Sendable.cpp
        src/Sendable/Sendable.h
        src/Sender/Sender.cpp
        src/Sender/Sender.h
        src/Exceptions/out_of_range_exception.cpp
        src/Exceptions/out_of_range_exception.h
        src/Reader/Reader.cpp
        src/Reader/Reader.h
        src/Reader/MyReader.cpp
        src/Reader/MyReader.h)

target_link_libraries(Pks_project ${Boost_LIBRARIES})

add_subdirectory(Tests)
add_subdirectory(ReaderProcess)