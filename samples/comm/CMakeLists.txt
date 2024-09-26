cmake_minimum_required(VERSION 3.16)
project(comm)

add_executable(${PROJECT_NAME}-reader reader.cpp)
target_link_libraries(${PROJECT_NAME}-reader
    pthread
    uuid
    coin-data
)

add_executable(${PROJECT_NAME}-writer writer.cpp)
target_link_libraries(${PROJECT_NAME}-writer
    pthread
    uuid
    coin-data
)

add_executable(${PROJECT_NAME}-publisher publisher.cpp)
target_link_libraries(${PROJECT_NAME}-publisher
    pthread
    uuid
    coin-data
)

add_executable(${PROJECT_NAME}-subscriber subscriber.cpp)
target_link_libraries(${PROJECT_NAME}-subscriber
    pthread
    uuid
    coin-data
)

add_executable(${PROJECT_NAME}-service service.cpp)
target_link_libraries(${PROJECT_NAME}-service
    pthread
    uuid
    coin-data
)

add_executable(${PROJECT_NAME}-client client.cpp)
target_link_libraries(${PROJECT_NAME}-client
    pthread
    uuid
    coin-data
)
