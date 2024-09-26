CMAKE_MINIMUM_REQUIRED(VERSION 3.12.0)
PROJECT(coin-project VERSION 1.0.0)

# 开启 C++17支持
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS ON)

SET(COIN_INSTALL_TOOLS "tools")

add_definitions(-DQT_NO_FOREACH)

# setup Qt
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)
FIND_PACKAGE(Qt5 COMPONENTS Core Widgets REQUIRED)

INCLUDE(${CMAKE_SOURCE_DIR}/cmake/coin_utils.cmake)
set(COIN_FILAMENT_ROOT $ENV{Coin_ROOT})
# =====================================================================
# 设置必要的 filament 变量
# =====================================================================
cmake_host_system_information(RESULT SYS_NA QUERY OS_PLATFORM)

set(COIN_FILAMENT_BIN_DIR $ENV{COIN_ROOT}/kit/filament/bin)
set(COIN_FILAMENT_LIB_DIR $ENV{COIN_ROOT}/kit/filament/lib/${CMAKE_SYSTEM_PROCESSOR})
set(COIN_FILAMENT_INC_DIR $ENV{COIN_ROOT}/kit/filament/include)
set(COIN_FILAMENT_MATC    $ENV{COIN_ROOT}/kit/filament/bin/matc)
set(COIN_FILAMENT_RESGEN  $ENV{COIN_ROOT}/kit/filament/bin/resgen)

set(COIN_FILAMENT_LIBRARIES 
    filament 
    backend
    bluegl
    bluevk
    filabridge
    filaflat
    utils
    geometry
    smol-v 
    vkshaders
    ibl
    camutils
    image
)

message(STATUS "CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")

INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coin-code/src
    ${CMAKE_CURRENT_SOURCE_DIR}/third_parts/tomlplusplus
    ${CMAKE_CURRENT_SOURCE_DIR}/third_parts/fmt/include
    ${COIN_FILAMENT_INC_DIR}
)
LINK_DIRECTORIES(
    ${COIN_FILAMENT_LIB_DIR}
)

set(BUILD_EXAMPLES ON)

ADD_SUBDIRECTORY(third_parts/nodeeditor)
# ADD_SUBDIRECTORY(third_part/tomlplusplus)
# 激活 header-only 模式
ADD_DEFINITIONS(-DFMT_HEADER_ONLY)
ADD_SUBDIRECTORY(third_parts/fmt)


ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(src)

# # 安装资源文件
# INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources
#     DESTINATION ${CMAKE_INSTALL_PREFIX}
# )

ADD_LIBRARY(${PROJECT_NAME} INTERFACE)

# 安装必要的脚本文件
FILE(GLOB_RECURSE pkg_files "${CMAKE_SOURCE_DIR}/package/*")
INSTALL(FILES ${pkg_files}
    DESTINATION ${CMAKE_INSTALL_BINDIR}/..
)

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake/CoinConfig-version.cmake.in
    ${CMAKE_BINARY_DIR}/cmake/CoinConfig-version.cmake
    @ONLY
)

# 安装 CoinConfig.cmake
INSTALL(FILES ${CMAKE_SOURCE_DIR}/cmake/CoinConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Coin
)
INSTALL(FILES ${CMAKE_BINARY_DIR}/cmake/CoinConfig-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Coin
)

# 将 cmake 目录中所有的 .cmake 文件安装到 ${CMAKE_INSTALL_LIBDIR}/cmake/Coin
file(GLOB cmake_files "${CMAKE_SOURCE_DIR}/cmake/*.cmake")
INSTALL(FILES ${cmake_files}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Coin
)

# 安装 data.co 目录
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/data.co DESTINATION ${CMAKE_INSTALL_BINDIR}/..)

########################################################################
# install #
######################################################################## 
install(TARGETS ${PROJECT_NAME} coin coin-commons coin-viz coin-node coin-data
    RUNTIME           # Following options apply to runtime artifacts.
        COMPONENT Runtime
    LIBRARY           # Following options apply to library artifacts.
        COMPONENT Runtime
        NAMELINK_COMPONENT Development
    ARCHIVE           # Following options apply to archive artifacts.
        COMPONENT Development
        DESTINATION lib
    PUBLIC_HEADER
        COMPONENT Development
        DESTINATION include
    PRIVATE_HEADER
        COMPONENT Development
        DESTINATION include
    FILE_SET HEADERS  # Following options apply to file set HEADERS.
        COMPONENT Development
        DESTINATION include
    FILE_SET DOCS
        COMPONENT Development
        DESTINATION docs
)

include(CPack)
