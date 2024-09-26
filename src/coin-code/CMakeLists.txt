cmake_minimum_required(VERSION 3.12)
project(coin-code)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/code_scan/config SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/code_scan/grammar SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/code_scan/type_tree SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/code_scan SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/code_tree SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/code_backend SRCS)

add_library(${PROJECT_NAME} SHARED ${SRCS})
target_include_directories(${PROJECT_NAME}
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(${PROJECT_NAME}
PRIVATE
    pthread
    coin-commons
)

INSTALL(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

file(GLOB_RECURSE COIN_PACK_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp)
foreach(COIN_PACK_HEADER_FILE ${COIN_PACK_HEADER_FILES})
    file(RELATIVE_PATH COIN_PACK_HEADER_FILE_RELATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src ${COIN_PACK_HEADER_FILE})
    # 获取文件目录
    get_filename_component(COIN_PACK_HEADER_FILE_DIR ${COIN_PACK_HEADER_FILE_RELATIVE_PATH} DIRECTORY)
    INSTALL(FILES ${COIN_PACK_HEADER_FILE} DESTINATION include/${PROJECT_NAME}/${COIN_PACK_HEADER_FILE_DIR})
endforeach()

include(CPack)
