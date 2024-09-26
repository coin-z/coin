# 


#[[
033[1;31;40m    <!--1-高亮显示 31-前景色红色  40-背景色黑色-->
033[0m          <!--采用终端默认设置，即取消颜色设置-->
显示方式  
0                终端默认设置
1                高亮显示
4                使用下划线
5                闪烁
7                反白显示
8                不可见

前景色            背景色           颜色
---------------------------------------
30                40              黑色
31                41              红色
32                42              绿色
33                43              黃色
34                44              蓝色
35                45              紫红色
36                46              青蓝色
37                47              白色
]]

string(ASCII 27 Esc)

# Esc[0;31m
set(R "${Esc}[0;31m")   #红色
set(Y "${Esc}[0;33m")   #黄色
set(G "${Esc}[0;32m")   #绿色
#Esc[0m 
set(E "${Esc}[m" )      #结束颜色设置
set(B "${Esc}[1;34m")   #蓝色高亮
set(RB "${Esc}[1;31;40m") #红色字体黑色背景


# 定义打印消息的函数
function(coin_info message)
    message("${G}${message}${E}")
endfunction()

function(coin_warn message)
    message("${Y}${message}${E}")
endfunction()

function(coin_error message)
    message("${R}${message}${E}")
endfunction()

function(coin_notice message)
    message("${B}${message}${E}")
endfunction()

function(coin_fatal message)
    message("${RB}${message}${E}")
endfunction()


function(coin_add_communicator TARGET)

    set(generated_path "${CMAKE_BINARY_DIR}/communicator")
    set(SOURCE_FILE_LIST ${ARGN})

    execute_process(
        COMMAND ${Coin_TOOL_DIR}/commc -t ${TARGET} -o ${generated_path} -i "${SOURCE_FILE_LIST}"
        OUTPUT_VARIABLE PROTOCOL_SOURCE_FILE_STR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # 使用分号分割的源文件列表
    string(REPLACE "\n" ";" PROTOCOL_SOURCE_FILE_LIST ${PROTOCOL_SOURCE_FILE_STR})

    add_custom_command(
        OUTPUT ${PROTOCOL_SOURCE_FILE_LIST}
        COMMAND ${Coin_TOOL_DIR}/commc -t ${TARGET} -o ${generated_path} -i "${SOURCE_FILE_LIST}"
        DEPENDS ${SOURCE_FILE_LIST}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Add communicator to target: ${TARGET}"
    )

    add_library(${TARGET} SHARED ${PROTOCOL_SOURCE_FILE_LIST})
    target_include_directories(${TARGET}
        PRIVATE
        "$ENV{Coin_ROOT}/include/"
        "$ENV{Coin_ROOT}/include/coin-data"
    )
    set(${TARGET}_INCLUDE_DIR ${generated_path} PARENT_SCOPE)

endfunction()

function(coin_add_data TARGET)

    set(generated_path "${CMAKE_BINARY_DIR}/${TARGET}.co")
    set(SOURCE_FILE_LIST ${ARGN})

    execute_process(
        COMMAND ${Coin_TOOL_DIR}/co-scan --target ${TARGET} --output ${generated_path} --input ${SOURCE_FILE_LIST}
        ECHO_OUTPUT_VARIABLE
        ECHO_ERROR_VARIABLE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

    # 使用分号分割的源文件列表
    # string(REPLACE "\n" ";" PROTOCOL_SOURCE_FILE_LIST ${PROTOCOL_SOURCE_FILE_STR})

    # coin_info(">>>${PROTOCOL_SOURCE_FILE_LIST}")
    # add_custom_command(
    #     OUTPUT ${PROTOCOL_SOURCE_FILE_LIST}
    #     COMMAND ${Coin_TOOL_DIR}/co-scan --target ${TARGET} --output ${generated_path} --input "${SOURCE_FILE_LIST}"
    #     DEPENDS ${SOURCE_FILE_LIST}
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    #     COMMENT "Add communicator to target: ${TARGET}"
    # )

    ADD_SUBDIRECTORY(${generated_path})
endfunction()


macro(coin_setup_filament)
    
    cmake_host_system_information(RESULT SYS_NA QUERY OS_PLATFORM)

    set(COIN_FILAMENT_BIN_DIR $ENV{COIN_ROOT}/kit/filament/bin)
    set(COIN_FILAMENT_LIB_DIR $ENV{COIN_ROOT}/kit/filament/lib/${CMAKE_SYSTEM_PROCESSOR})
    set(COIN_FILAMENT_INC_DIR $ENV{COIN_ROOT}/kit/filament/include)
    set(COIN_FILAMENT_MATC    $ENV{COIN_ROOT}/kit/filament/bin/matc)
    set(COIN_FILAMENT_RESGEN  $ENV{COIN_ROOT}/kit/filament/bin/resgen)

    file(GLOB LIBRARY_FILES "${COIN_FILAMENT_LIB_DIR}/*.a")

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

    # foreach(LIBRARY_FILE ${LIBRARY_FILES})
    #     get_filename_component(LIBRARY_NAME ${LIBRARY_FILE} NAME_WE)
    #     string(REGEX REPLACE "^lib" "" LIBRARY_NAME ${LIBRARY_NAME})
    #     list(APPEND COIN_FILAMENT_LIBRARIES ${LIBRARY_NAME})
    # endforeach()
    # unset(LIBRARY_FILE)

endmacro(coin_setup_filament)


function(coin_add_materials mat_name materials)

    set(GENERATION_ROOT ${CMAKE_CURRENT_BINARY_DIR}/generated/filament)
    set(RESOURCE_DIR  "${GENERATION_ROOT}/resources")
    set(MATERIAL_DIR  "${GENERATION_ROOT}/material")
    set(TEXTURE_DIR "${GENERATION_ROOT}/texture")
    set(${mat_name}_MATERIAL_INC_DIR "${RESOURCE_DIR}" PARENT_SCOPE)
    set(RESOURCE_BINS)

    if(NOT EXISTS "${RESOURCE_DIR}")
        file(MAKE_DIRECTORY "${RESOURCE_DIR}")
    endif()
    if(NOT EXISTS "${MATERIAL_DIR}")
        file(MAKE_DIRECTORY "${MATERIAL_DIR}")
    endif()
    if(NOT EXISTS "${TEXTURE_DIR}")
        file(MAKE_DIRECTORY "${TEXTURE_DIR}")
    endif()

    message(STATUS ">>matc: ${COIN_FILAMENT_MATC}")
    message(STATUS ">>resgen: ${COIN_FILAMENT_RESGEN}")

    foreach (mat_src ${materials})
        get_filename_component(localname "${mat_src}" NAME_WE)
        get_filename_component(fullname "${mat_src}" ABSOLUTE)
        set(output_path "${MATERIAL_DIR}/${localname}.filamat")
        add_custom_command(
            OUTPUT ${output_path}
            COMMAND ${COIN_FILAMENT_MATC} ${MATC_BASE_FLAGS} -a vulkan -o ${output_path} ${fullname}
            MAIN_DEPENDENCY ${mat_src}
            # DEPENDS matc
            COMMENT "Compiling material ${mat_src} to ${output_path}"
        )
        list(APPEND RESOURCE_BINS ${output_path})
    endforeach()

    string(REPLACE "-" "_" ARCHIVE_NAME ${mat_name})
    set(ARCHIVE_DIR ${RESOURCE_DIR})
    set(OUTPUTS
        ${ARCHIVE_DIR}/${ARCHIVE_NAME}.bin
        ${ARCHIVE_DIR}/${ARCHIVE_NAME}.S
        ${ARCHIVE_DIR}/${ARCHIVE_NAME}.apple.S
        ${ARCHIVE_DIR}/${ARCHIVE_NAME}.h
    )
    if (IOS)
        set(ASM_ARCH_FLAG "-arch ${DIST_ARCH}")
    endif()
    if (APPLE)
        set(ASM_SUFFIX ".apple")
    endif()
    set(RESGEN_HEADER "${ARCHIVE_DIR}/${ARCHIVE_NAME}.h")
    # Visual Studio makes it difficult to use assembly without using MASM. MASM doesn't support
    # the equivalent of .incbin, so on Windows we'll just tell resgen to output a C file.
    if (WEBGL OR WIN32 OR ANDROID_ON_WINDOWS)
        set(RESGEN_OUTPUTS "${OUTPUTS};${ARCHIVE_DIR}/${ARCHIVE_NAME}.c")
        set(RESGEN_FLAGS -qcx ${ARCHIVE_DIR} -p ${ARCHIVE_NAME})
        set(RESGEN_SOURCE "${ARCHIVE_DIR}/${ARCHIVE_NAME}.c")
    else()
        set(RESGEN_OUTPUTS "${OUTPUTS}")
        set(RESGEN_FLAGS -qx ${ARCHIVE_DIR} -p ${ARCHIVE_NAME})
        set(RESGEN_SOURCE "${ARCHIVE_DIR}/${ARCHIVE_NAME}${ASM_SUFFIX}.S")
        set(RESGEN_SOURCE_FLAGS "-I${ARCHIVE_DIR} ${ASM_ARCH_FLAG}")
    endif()

    add_custom_command(
        OUTPUT ${RESGEN_OUTPUTS}
        COMMAND ${COIN_FILAMENT_RESGEN} ${RESGEN_FLAGS} ${RESOURCE_BINS}
        DEPENDS ${RESOURCE_BINS}
        COMMENT "Aggregating resources"
    )

    if (DEFINED RESGEN_SOURCE_FLAGS)
        set_source_files_properties(${RESGEN_SOURCE} PROPERTIES COMPILE_FLAGS ${RESGEN_SOURCE_FLAGS})
    endif()

    # CMake fails to invoke ar on Windows unless there is at least one C/C++ file in the library.
    set(DUMMY_SRC "${RESOURCE_DIR}/dummy.c")

    add_custom_command(OUTPUT ${DUMMY_SRC} COMMAND echo "//" > ${DUMMY_SRC})

    coin_info(">>>> resgen source: ${RESGEN_SOURCE}")
    coin_info(">>>> mat name: ${mat_name}")
    add_library(${mat_name} ${DUMMY_SRC} ${RESGEN_SOURCE})
    set_target_properties(${mat_name} PROPERTIES FOLDER Samples/Resources)
    target_include_directories(${mat_name}
    PRIVATE
        ${RESOURCE_DIR}
    )

    unset(RESGEN_OUTPUTS)
    unset(RESGEN_FLAGS)
    unset(RESGEN_SOURCE)
    unset(RESGEN_SOURCE_FLAGS)

    unset(GENERATION_ROOT)
    unset(RESOURCE_DIR)
    unset(MATERIAL_DIR)
    unset(TEXTURE_DIR)
    unset(RESOURCE_BINS)

endfunction()


function(_coin_read_module_info input_string _cate _name _version)
    # 检查是否包含"::"，如果包含则提取model、name和version
    # 支持一下写法：
    #   1. <cate>::<name>@<version>
    #   2. <cate>::<name>
    #   3. <name>@<version>
    #   4. <name>

    # 检查是否存在 cate 字段
    string(FIND ${input_string} "::" has_cate)
    # 检查是否存在 version 字段
    string(FIND ${input_string} "@" has_version)
    if(${has_cate} GREATER -1)
        string(REGEX MATCH "([^:]*)::([^@]*)" _ ${input_string})
        set(${_name} "${CMAKE_MATCH_2}" PARENT_SCOPE)
        set(${_cate} "${CMAKE_MATCH_1}" PARENT_SCOPE)
    endif(${has_cate} GREATER -1)

    if(${has_version} GREATER -1)
        string(REGEX MATCH "([^@]*)@([^@]*)" _ ${input_string})
        set(${_name} "${CMAKE_MATCH_1}" PARENT_SCOPE)
        set(${_version} "${CMAKE_MATCH_2}" PARENT_SCOPE)
    endif(${has_version} GREATER -1)

    if(NOT ${has_cate} GREATER -1 AND NOT ${has_version} GREATER -1)
        set(${_name} "${input_string}" PARENT_SCOPE)
    endif(NOT ${has_cate} GREATER -1 AND NOT ${has_version} GREATER -1)
endfunction()

function(_coin_convert_cate_to_path input_cate output_path)
    string(REPLACE "." "/" ${input_cate} ${output_path})
endfunction()
