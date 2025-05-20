set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
cmake_minimum_required(VERSION 3.26)

# specify cross-compilers and tools
set(CMAKE_C_COMPILER loongarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER loongarch64-linux-gnu-g++)
set(CMAKE_ASM_COMPILER loongarch64-linux-gnu-gcc)
set(CMAKE_AR loongarch64-linux-gnu-ar)
set(CMAKE_OBJCOPY loongarch64-linux-gnu-objcopy)
set(CMAKE_OBJDUMP loongarch64-linux-gnu-objdump)
set(SIZE loongarch64-linux-gnu-size)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# project settings
project(GUI C CXX ASM)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

add_compile_options(-Og)

include_directories(
        Inc
)

add_definitions()

file(GLOB_RECURSE SOURCES "Src/*.*")

set(LINKER_SCRIPT)

add_executable(${PROJECT_NAME} ${SOURCES} ${LINKER_SCRIPT})

