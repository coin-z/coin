# coin

# ======================================================
#  Project variables:
# ======================================================
SET(Coin_PROJECT_NAME "Coin")
SET(Coin_PROJECT_DESCRIPTION "Coin is a toy tool for code impressive things.")

include(${CMAKE_CURRENT_LIST_DIR}/coin_utils.cmake)

# ======================================================
#  Project options:
# ======================================================
# include deirectories
SET(Coin_INCLUDE_DIR $ENV{COIN_ROOT}/include)
file(GLOB SUBDIRECTORIES "$ENV{COIN_ROOT}/include/*")
foreach(SUBDIRECTORY ${SUBDIRECTORIES})
    if(IS_DIRECTORY ${SUBDIRECTORY})
        list(APPEND Coin_INCLUDE_DIR ${SUBDIRECTORY})
    endif()
endforeach()

# libs
link_directories("$ENV{COIN_ROOT}/lib")
list(APPEND Coin_LIBRARIES coin-commons)

# 遍历COMPONENTS列表并打印每个组件的信息
# https://cmake.org/cmake/help/latest/command/find_package.html
foreach(component ${Coin_FIND_COMPONENTS})
    message(STATUS "Component: ${component}")
    
    SET(COMP_CATE)
    SET(COMP_NAME)
    SET(COMP_VERSION)

    _coin_read_module_info(${component} COMP_CATE COMP_NAME COMP_VERSION)

    SET(comp_path)

    if(NOT COMP_CATE AND NOT COMP_VERSION)
        list(APPEND Coin_LIBRARIES coin-${component})
    elseif(${COMP_CATE} STREQUAL "kit")

        # 检查有效性
        set(_COMP_PATH $ENV{Coin_ROOT}/kit/${COMP_NAME})
        string(TOLOWER "${COMP_NAME}" LOWER_COMP_NAME)
        
        if(NOT EXISTS ${_COMP_PATH})
            set(_COMP_PATH $ENV{Coin_ROOT}/kit/${LOWER_COMP_NAME})
            if(NOT EXISTS ${_COMP_PATH})
                string(TOUPPER "${COMP_NAME}" UPPER_COMP_NAME)
                set(_COMP_PATH $ENV{Coin_ROOT}/kit/${UPPER_COMP_NAME})
            endif(NOT EXISTS ${_COMP_PATH})
        endif(NOT EXISTS ${_COMP_PATH})

        coin_info("${COMP_NAME} FIND PATH: ${_COMP_PATH}")
        if(${LOWER_COMP_NAME} STREQUAL "eigen3")
            set(COMP_FIND_NAME Eigen3)
        else()
            set(COMP_FIND_NAME ${COMP_NAME})
        endif()

        if(EXISTS ${_COMP_PATH})
            find_package(${COMP_FIND_NAME}
                PATHS ${_COMP_PATH} NO_DEFAULT_PATH
                REQUIRED
            )
        else()
            find_package(${COMP_FIND_NAME} REQUIRED)
        endif()
        list(APPEND Coin_LIBRARIES ${${COMP_NAME}_LIBRARIES})
        list(APPEND Coin_INCLUDE_DIR ${${COMP_NAME}_INCLUDE_DIR})
    else()
        _coin_convert_cate_to_path(${COMP_CATE} comp_path)
        find_package(coin-${component}
            PATHS ${comp_path} NO_DEFAULT_PATH
        )
    endif()
endforeach()

# tools deirectories
SET(Coin_TOOL_DIR "$ENV{COIN_ROOT}/tools")

include(${CMAKE_CURRENT_LIST_DIR}/coin_utils.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/coin_pack.cmake)
