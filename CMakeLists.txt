cmake_minimum_required(VERSION 3.22)

project(CoreRTOS VERSION 1.0 LANGUAGES C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scheduler.c
)

add_library(CoreRTOS STATIC ${SOURCES})


target_include_directories(CoreRTOS
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Add an executable target for testing (main.c)
add_executable(CoreRTOS_Test
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
)

# Link the CoreRTOS library to the test executable
target_link_libraries(CoreRTOS_Test CoreRTOS)