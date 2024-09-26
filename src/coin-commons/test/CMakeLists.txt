cmake_minimum_required(VERSION 3.16)
project(coin-commons-test)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/executor EXECUTOR_SRCS)

add_executable(executor-test ${EXECUTOR_SRCS})
target_link_libraries(executor-test
    coin-commons
)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/dummy DUMMY_SRCS)
add_executable(dummy-test ${DUMMY_SRCS})
target_link_libraries(dummy-test
    coin-commons
)
