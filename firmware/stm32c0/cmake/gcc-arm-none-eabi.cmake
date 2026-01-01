# cmake/gcc-arm-none-eabi.cmake

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Absolute path to ARM GNU Toolchain
set(TOOLCHAIN_DIR
    /Applications/ArmGNUToolchain/15.2.rel1/arm-none-eabi/bin
)
set(TOOLCHAIN_PREFIX
    ${TOOLCHAIN_DIR}/arm-none-eabi-
)

# Compilers
set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)

# Binutils
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE    ${TOOLCHAIN_PREFIX}size)
set(CMAKE_OBJDUMP ${TOOLCHAIN_PREFIX}objdump)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# MCU flags (STM32C0 = Cortex-M0+)
set(MCU_FLAGS "-mcpu=cortex-m0plus -mthumb")

# Common flags
set(COMMON_FLAGS "${MCU_FLAGS} -ffunction-sections -fdata-sections -Wall -Wextra -Wpedantic")

set(CMAKE_C_FLAGS   "${COMMON_FLAGS}")
set(CMAKE_ASM_FLAGS "${COMMON_FLAGS} -x assembler-with-cpp")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -fno-rtti -fno-exceptions")

# Build types
set(CMAKE_C_FLAGS_DEBUG   "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os -g0")

set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} --specs=nano.specs -Wl,--gc-sections -Wl,--print-memory-usage -lc -lm ")
