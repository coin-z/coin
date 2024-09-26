cmake_minimum_required(VERSION 3.16)
project(utils)

add_executable(${PROJECT_NAME}-mut-sub mut_sub.cpp)
target_link_libraries(${PROJECT_NAME}-mut-sub
    pthread
    uuid
    coin-data
)

add_executable(${PROJECT_NAME}-redirect redirect.cpp)
target_link_libraries(${PROJECT_NAME}-redirect
    pthread
    uuid
    coin-data
)
