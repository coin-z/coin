cmake_minimum_required(VERSION 3.12)
project(communicator)

add_executable(${PROJECT_NAME} ./communicator.cpp)
target_link_libraries(${PROJECT_NAME} 
    pthread
    util
)
