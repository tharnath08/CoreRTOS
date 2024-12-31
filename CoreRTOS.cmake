cmake_minimum_required(VERSION 3.22)

project(CoreRTOS VERSION 1.0 LANGUAGES C)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

set(SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/scheduler.c
)

add_library(CoreRTOS STATIC ${SOURCES})


target_include_directories(CoreRTOS
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
)
