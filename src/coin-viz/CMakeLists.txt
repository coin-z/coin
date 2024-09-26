cmake_minimum_required(VERSION 3.16.0)

project(coin-viz VERSION 1.0.0 LANGUAGES CXX C ASM)

# set(CMAKE_C_COMPILER /usr/bin/clang-12)
# set(CMAKE_CXX_COMPILER /usr/bin/clang++-12)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(Vulkan REQUIRED)


file(GLOB MAT_FAILES "${CMAKE_CURRENT_SOURCE_DIR}/materials/*.mat")
set(GENERATION_ROOT ${CMAKE_CURRENT_BINARY_DIR}/generated/filament)
set(RESOURCE_DIR  "${GENERATION_ROOT}/resources")
set(MATERIAL_DIR  "${GENERATION_ROOT}/material")
set(TEXTURE_DIR "${GENERATION_ROOT}/texture")

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

foreach (mat_src ${MAT_FAILES})
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

set(ARCHIVE_NAME resources)
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
add_library(${ARCHIVE_NAME} STATIC ${DUMMY_SRC} ${RESGEN_SOURCE})
set_target_properties(${ARCHIVE_NAME} PROPERTIES FOLDER Samples/Resources)
target_include_directories(${ARCHIVE_NAME}
PRIVATE
    ${RESOURCE_DIR}
)

unset(RESGEN_OUTPUTS)
unset(RESGEN_FLAGS)
unset(RESGEN_SOURCE)
unset(RESGEN_SOURCE_FLAGS)

unset(GENERATION_ROOT)
# unset(RESOURCE_DIR)
unset(MATERIAL_DIR)
unset(TEXTURE_DIR)
unset(RESOURCE_BINS)




# file(GLOB MAT_FAILES "${CMAKE_CURRENT_SOURCE_DIR}/materials/*.mat")
# coin_add_materials(resources "${MAT_FAILES}")
message(STATUS ">>>>>>>>>>>>>>>>${RESOURCE_DIR}")
message(STATUS ">>>>>>>>>>>>>>>>${COIN_FILAMENT_INC_DIR}")
include_directories(${COIN_FILAMENT_INC_DIR})
# ==================================================================================================
# Build target
# ==================================================================================================
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRCS)

add_library(${PROJECT_NAME} SHARED ${SRCS})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${COIN_FILAMENT_INC_DIR}
    ${RESOURCE_DIR}
)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${COIN_FILAMENT_LIB_DIR}
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Widgets

    ${COIN_FILAMENT_LIBRARIES}

    resources

    -lc++
    -lc++abi
    -pthread
    -ldl
    -lSDL2

    ${Vulkan_LIBRARIES}
)
