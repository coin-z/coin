# coin pack function and macro

# ==============================================================
# @brief   coin pack function
# @param   options
# @param   oneValueArgs
# @param   multiValueArgs
# @param   ARGN
# @return  none
# @note    none
# ==============================================================
function(coin_pack)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs ARGS)
    cmake_parse_arguments(COIN_PACK "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # 计算打包缓存目录
    set(COIN_PACK_CACHE_DIR "${CMAKE_BINARY_DIR}/.coin_pack")
    # 检查 COIN_PACK_CACHE_DIR 是否存在，不存在则创建
    if(NOT EXISTS ${COIN_PACK_CACHE_DIR})
        coin_info("coin_pack: create cache dir: ${COIN_PACK_CACHE_DIR}")
        file(MAKE_DIRECTORY ${COIN_PACK_CACHE_DIR})
    endif()

    # 安装工程
    INSTALL(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${COIN_PACK_CACHE_DIR}/bin
        LIBRARY DESTINATION ${COIN_PACK_CACHE_DIR}/lib
        ARCHIVE DESTINATION ${COIN_PACK_CACHE_DIR}/lib
    )

    # 处理 h/hpp文件
    # 查找 src 目录下的全部 h/hpp 文件
    file(GLOB_RECURSE COIN_PACK_HEADER_FILES ${CMAKE_SOURCE_DIR}/src/*.h ${CMAKE_SOURCE_DIR}/src/*.hpp)
    foreach(COIN_PACK_HEADER_FILE ${COIN_PACK_HEADER_FILES})
        file(RELATIVE_PATH COIN_PACK_HEADER_FILE_RELATIVE_PATH ${CMAKE_SOURCE_DIR}/src ${COIN_PACK_HEADER_FILE})
        coin_notice("coin_pack: ${COIN_PACK_HEADER_FILE_RELATIVE_PATH}")
        # 获取文件目录
        get_filename_component(COIN_PACK_HEADER_FILE_DIR ${COIN_PACK_HEADER_FILE_RELATIVE_PATH} DIRECTORY)
        install(FILES ${COIN_PACK_HEADER_FILE} DESTINATION ${COIN_PACK_CACHE_DIR}/include/${COIN_PACK_HEADER_FILE_DIR})
    endforeach()

    # 处理 cmake 文件
    file(GLOB_RECURSE COIN_PACK_CMAKE_FILES ${CMAKE_SOURCE_DIR}/cmake/*.cmake)
    foreach(COIN_PACK_CMAKE_FILE ${COIN_PACK_CMAKE_FILES})
        file(RELATIVE_PATH COIN_PACK_CMAKE_FILE_RELATIVE_PATH ${CMAKE_SOURCE_DIR} ${COIN_PACK_CMAKE_FILE})
        install(FILES ${COIN_PACK_CMAKE_FILE} DESTINATION ${COIN_PACK_CACHE_DIR}/cmake)
    endforeach()

    # 处理 doc 文件，认为 md 文件为有效 doc 文件
    file(GLOB_RECURSE COIN_PACK_DOCMD_FILES ${CMAKE_SOURCE_DIR}/*.md)
    foreach(COIN_PACK_DOCMD_FILE ${COIN_PACK_DOCMD_FILES})
        file(RELATIVE_PATH COIN_PACK_DOCMD_FILE_RELATIVE_PATH ${CMAKE_SOURCE_DIR} ${COIN_PACK_DOCMD_FILE})
        get_filename_component(COIN_PACK_DOCMD_FILE_DIR ${COIN_PACK_DOCMD_FILE_RELATIVE_PATH} DIRECTORY)
        install(FILES ${COIN_PACK_DOCMD_FILE} DESTINATION ${COIN_PACK_CACHE_DIR}/doc/${COIN_PACK_DOCMD_FILE_DIR})
    endforeach()

    # 处理 package 目录
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/.package DESTINATION ${COIN_PACK_CACHE_DIR})
    

endfunction()
