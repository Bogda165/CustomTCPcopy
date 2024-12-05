cmake_minimum_required(VERSION 3.29)

# Define the Protocol library
add_library(Protocol STATIC
        Socket/MySocket.cpp
        Socket/MySocket.h
        Data/data.cpp
        Data/data.h
        Packet/header.cpp
        Packet/header.h
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
        Sender/Sender.h
        Exceptions/out_of_range_exception.cpp
        Exceptions/out_of_range_exception.h
        Reader/Reader.cpp
        Reader/Reader.h
        Reader/MyReader.cpp
        Reader/MyReader.h
        Data/data_i.cpp
        Data/data_i.h
        Data/DinamicData.cpp
        Data/DinamicData.h
        Data/FileData.cpp
        Data/FileData.h
        ShowObserver/showObserver.cpp
        ShowObserver/showObserver.h
        ShowObserver/showObserverTerminal.cpp
        ShowObserver/showObserverTerminal.h
        KeepAlive/keepAlive.cpp
        KeepAlive/keepAlive.h
        ShowObserver/showObserverPipe.cpp
        ShowObserver/showObserverPipe.h
        # Add other Protocol-specific source files here if necessary
)

# Link Boost if needed by Protocol
find_package(Boost 1.70 REQUIRED COMPONENTS system)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif()

# Include Boost headers and other needed directories
target_include_directories(Protocol PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(Protocol PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Reader ${CMAKE_CURRENT_SOURCE_DIR}/Socket)
target_link_libraries(Protocol PRIVATE ${Boost_LIBRARIES})

# Set the output directory for the Protocol library
set_target_properties(Protocol PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)