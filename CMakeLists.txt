cmake_minimum_required(VERSION 3.29)
project(Pks_project)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.70 REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif()

include_directories(${Boost_INCLUDE_DIRS})

add_executable(Pks_project main.cpp
        Socket/MySocket.cpp
        Socket/MySocket.h
        Data/data.cpp
        Data/data.h
        Socket/header.cpp
        Socket/header.h
        Receivers/receiver.cpp
        Receivers/receiver.h
        Receivers/myReceiver.cpp
        Receivers/myReceiver.h
        Socket/HandShakeStats.cpp
        Socket/HandShakeStats.h
        Packet/Packet.cpp
        Packet/Packet.h
        WindowF/WindowF.cpp
        WindowF/WindowF.h
        WindowF/MyWindow.cpp
        WindowF/MyWindow.h
        Sendable/Sendable.cpp
        Sendable/Sendable.h
        Sender/Sender.cpp
        Sender/Sender.h)

target_link_libraries(Pks_project ${Boost_LIBRARIES})