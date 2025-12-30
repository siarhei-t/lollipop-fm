cmake_minimum_required(VERSION 3.22)

set(CMAKE_PROJECT_NAME stm32c011)

project(${CMAKE_PROJECT_NAME})
add_library(${CMAKE_PROJECT_NAME} INTERFACE)

target_compile_definitions(${CMAKE_PROJECT_NAME} INTERFACE 
    STM32C011xx
)

target_include_directories(${CMAKE_PROJECT_NAME} INTERFACE
    ./
    inc
)

target_sources(${CMAKE_PROJECT_NAME} INTERFACE
    system_stm32c0xx.c
    startup_stm32c011xx.s
)