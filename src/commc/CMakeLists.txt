CMAKE_MINIMUM_REQUIRED(VERSION 3.12.0)
PROJECT(commc VERSION 1.0.0)

# 开启 C++17支持
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS ON)


include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.0
)
FetchContent_MakeAvailable(argparse)

add_executable(${PROJECT_NAME} ./commc.cpp)
target_link_libraries(${PROJECT_NAME}
    coin-code
)


add_executable(co-scan ./co-scan.cpp)
target_include_directories(co-scan
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(co-scan
PRIVATE
    pthread
    coin-code
    coin-commons
    argparse
)

INSTALL(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${COIN_INSTALL_TOOLS}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

INSTALL(TARGETS co-scan
    RUNTIME DESTINATION ${COIN_INSTALL_TOOLS}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

include(CPack)
