cmake_minimum_required(VERSION 3.23)
project(data)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coin-data
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coin-data/local
    ${CMAKE_CURRENT_SOURCE_DIR}/third_parts
    ${CMAKE_CURRENT_SOURCE_DIR}/third_parts/asio
)

add_subdirectory(src)

add_library(${PROJECT_NAME} INTERFACE)
