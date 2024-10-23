cmake_minimum_required(VERSION 3.29)
project(Pks_project)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost 1.70 REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif()

include_directories(${Boost_INCLUDE_DIRS})

add_executable(Pks_project main.cpp
        MySocket.cpp
        MySocket.h
        data.cpp
        data.h
        header.cpp
        header.h)

target_link_libraries(Pks_project ${Boost_LIBRARIES})