cmake_minimum_required(VERSION 3.9)
project(event_reader C)

set(CMAKE_C_STANDARD 99)

add_executable(event_reader mouse.c presskeys.c)