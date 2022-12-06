cmake_minimum_required(VERSION 3.20)
project(bnn
        LANGUAGES C
        VERSION 1.0.0
)

set(CMAKE_C_STANDARD 99)
file(GLOB C_SOURCES ./src/*.c ./examples/*.c)

add_executable(bnn ${C_SOURCES})
target_include_directories(bnn PUBLIC ${CMAKE_SOURCE_DIR}/inc ${CMAKE_SOURCE_DIR}/examples)