cmake_minimum_required(VERSION 3.23)
project(coin-data)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}            SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/local      SRCS)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}/local/impl SRCS)

file(GLOB_RECURSE HEADER_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(${PROJECT_NAME} SHARED ${SRCS})
target_sources(${PROJECT_NAME}
    PUBLIC FILE_SET HEADERS FILES "${HEADER_FILES}"
)
target_link_libraries(${PROJECT_NAME}
    coin-commons
    pthread
    rt
    uuid
)
target_include_directories(${PROJECT_NAME}
PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
  FRAMEWORK TRUE
  FRAMEWORK_VERSION C
  MACOSX_FRAMEWORK_IDENTIFIER com.cmake.${PROJECT_NAME}
  MACOSX_FRAMEWORK_INFO_PLIST Info.plist
  # "current version" in semantic format in Mach-O binary file
  VERSION 1.0.0
  # "compatibility version" in semantic format in Mach-O binary file
  SOVERSION 1.0.0
)

# export(TARGETS ${PROJECT_NAME} NAMESPACE coin:: FILE ${PROJECT_NAME}.cmake)
