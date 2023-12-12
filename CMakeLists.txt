cmake_minimum_required(VERSION 3.0)

project(xhell)

set(CMAKE_C_STANDARD 11)

set(SOURCE_DIR src)
set(INCLUDE_DIR include)

file(GLOB SOURCES "${SOURCE_DIR}/*.c")

include_directories(${INCLUDE_DIR})

add_executable(xhell ${SOURCES})

