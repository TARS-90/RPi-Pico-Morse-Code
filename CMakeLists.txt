cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
include(pico_sdk_import.cmake)

project(pico)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of project
add_executable(program program.c)

# add pico stdlib library which aggregates commonly used features
target_link_libraries(program pico_stdlib)

# create map/bin/hex/uf2 files in addition to ELF
pico_add_extra_outputs(program)

pico_enable_stdio_usb(program 1)
pico_enable_stdio_uart(program 0)
