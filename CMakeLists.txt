cmake_minimum_required(VERSION 3.23)
project(udp_file_transfer)

set(CMAKE_CXX_STANDARD 20)

add_executable(udp_file_transfer main.cpp file_sender.cpp file_sender.h file_receiver.cpp file_receiver.h constants.h)
