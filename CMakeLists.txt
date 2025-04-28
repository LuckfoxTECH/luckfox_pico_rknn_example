cmake_minimum_required(VERSION 3.10)

#Processing Parameters
if(DEFINED LIBC_TYPE)
    message(STATUS "Selected libc type: ${LIBC_TYPE}")
else()
    message(FATAL_ERROR "No libc type selected!")
    return()
endif()

set(SDK_PATH $ENV{LUCKFOX_SDK_PATH})
set(GLIBC_COMPILER $ENV{GLIBC_COMPILER})
if(${LIBC_TYPE} STREQUAL "uclibc")
    if (NOT SDK_PATH)
        message(WARNING "Please Set Luckfox-pico SDK Path. Such as:export LUCKFOX_SDK_PATH=/home/user/luckfox-pico")
        return()
    else()
        set(CMAKE_C_COMPILER "${SDK_PATH}/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-gcc")
        set(CMAKE_CXX_COMPILER "${SDK_PATH}/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-g++")
    endif()
elseif(${LIBC_TYPE} STREQUAL "glibc")
    if (NOT GLIBC_COMPILER)
        message(WARNING "Please Set arm32 glibc compiler Path. Such as:export GLIBC_COMPILER=/opt/arm-linux-gnueabihf/bin/arm-linux-gnueabihf-")
        return()
    else()
        set(CMAKE_C_COMPILER    "${GLIBC_COMPILER}gcc")
        set(CMAKE_CXX_COMPILER  "${GLIBC_COMPILER}g++")
    endif()
endif()

option(USE_LUCKFOX_PICO_PLUS "Enable LUCKFOX_PICO_PLUS feature" OFF)
option(USE_LUCKFOX_PICO_PRO_MAX "Enable LUCKFOX_PICO_PRO_MAX feature" OFF)

if(DEFINED EXAMPLE_DIR)
    message(STATUS "Selected source directory: ${EXAMPLE_DIR}")
else()
    message(FATAL_ERROR "No source directory selected!")
    return()
endif()

if(DEFINED EXAMPLE_NAME)
    message(STATUS "Set project name: ${EXAMPLE_NAME}")
else()
    message(FATAL_ERROR "Project name not specified!")
    return()
endif()

if(LUCKFOX_PICO_PLUS)
    add_definitions(-DLUCKFOX_PICO_PLUS)
    message(STATUS "Set Device: Luckfox Pico Plus")
endif()

if(LUCKFOX_PICO_PRO_MAX)
    add_definitions(-DLUCKFOX_PICO_PRO_MAX)
    message(STATUS "Set Device: Luckfox Pico Pro Max")
endif()

project(${EXAMPLE_NAME})

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/${LIBC_TYPE})

add_definitions(-DRV1106_1103)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/Config)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/GUI)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/LCD)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/SPI)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/GPIO)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ 3rdparty.out)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utils/ utils.out)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/Config)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/GUI)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/LCD)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/SPI)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/GPIO)

set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")

#Opencv 4 
set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/${LIBC_TYPE}/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} )

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(SRC_DIR "${EXAMPLE_DIR}/src")
file(GLOB SRC_FILES "${SRC_DIR}/*.cc")
add_executable(${PROJECT_NAME} ${SRC_FILES})

if(${LIBC_TYPE} STREQUAL "uclibc")
    target_link_libraries(${PROJECT_NAME}
        ${OpenCV_LIBS}
        rknnmrt
        Threads::Threads
        imageutils
        fileutils
        imagedrawing
        Config
        GUI
        LCD
        SPI
        GPIO
    )
elseif(${LIBC_TYPE} STREQUAL "glibc")
    target_link_libraries(${PROJECT_NAME}
        ${OpenCV_LIBS}
        rknnmrt
        Threads::Threads
        imageutils
        fileutils
        imagedrawing
        Config
        GUI
        LCD
        SPI
        GPIO
        stdc++
        func_none
        c
        m
        rt
        dl
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${EXAMPLE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/rknn
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/librga/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/allocator/dma
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/Config
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/GUI 
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/LCD
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/SPI
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/GPIO 
    ${OpenCV_INCLUDE_DIRS}
)

#Install
if(LUCKFOX_PICO_PLUS)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install/${LIBC_TYPE}/${PROJECT_NAME}_plus_demo")
elseif(LUCKFOX_PICO_PRO_MAX)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install/${LIBC_TYPE}/${PROJECT_NAME}_pro_max_demo")
else()
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install/${LIBC_TYPE}/${PROJECT_NAME}_demo")
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
SET(MODEL_DIR "${EXAMPLE_DIR}/model")
file(GLOB MODEL_FILES "${MODEL_DIR}/*")  
install(FILES ${MODEL_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/model)                    