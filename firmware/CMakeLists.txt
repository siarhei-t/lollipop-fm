cmake_minimum_required(VERSION 3.22)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME lollipop-fm.elf)

# Include toolchain file
include("stm32c0/cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

message("Build type: " ${CMAKE_BUILD_TYPE})

set(FREERTOS_KERNEL_PATH "modules/freertos")
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config
    INTERFACE
    "inc"
)

# Select the heap port.  values between 1-4 will pick a heap.
set(FREERTOS_HEAP "1" CACHE STRING "" FORCE)

# Select the native compile PORT
set(FREERTOS_PORT "GCC_ARM_CM0" CACHE STRING "" FORCE)

# Adding the FreeRTOS-Kernel subdirectory
add_subdirectory(${FREERTOS_KERNEL_PATH} FreeRTOS-Kernel)

# Add STM32C0 startup and system init
add_subdirectory(stm32c0)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    src/syscalls.c
    src/main.cpp
    src/app.cpp
    src/led.cpp
    src/serial.cpp
    src/frequency_meter.cpp
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE

)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32c011
    freertos_kernel
    freertos_config
)

# linker script
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    -T${CMAKE_CURRENT_SOURCE_DIR}/stm32c0/STM32C011F6UX_FLASH.ld
)
