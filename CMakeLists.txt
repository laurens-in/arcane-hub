cmake_minimum_required(VERSION 3.20.0)
# Set toolchain file before the project command
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain/arm_gcc.cmake)
project(
    arcane
    VERSION 1.0
    LANGUAGES C ASM
)

set(CMAKE_BUILD_TYPE_INIT "Release")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Build types" FORCE)

set(MCU_VARIANT nrf52840)
set(LD_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linker/${MCU_VARIANT}.ld)
set(CMAKE_EXECUTABLE_SUFFIX .elf)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4 CACHE INTERNAL "System Processor")

add_executable(${PROJECT_NAME})

# Set compile options based on the build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Configuring Debug build")
    target_compile_options(${PROJECT_NAME} PUBLIC -g -O0)
else()
    message(STATUS "Configuring Release build")
    target_compile_options(${PROJECT_NAME} PUBLIC -O3)
endif()

target_link_options(${PROJECT_NAME} PUBLIC
    "LINKER:--script=${LD_FILE}"
    -L${CMAKE_CURRENT_LIST_DIR}/external/nrfx/mdk
    --specs=nosys.specs --specs=nano.specs
)

set(SD_NAME s140)
set(DFU_DEV_REV 52840)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    NRF52840_XXAA
    S140
)

# Create an interface library for external dependencies
add_library(external_libs INTERFACE)

# Set include directories for external libraries (only for external_libs)
target_include_directories(external_libs INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/hw
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/hw/bsp/nrf
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/hw/bsp/nrf/boards/feather_nrf52840_express
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/class/cdc/
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/drivers/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/drivers/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/hal
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/mdk
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/helpers
    ${CMAKE_CURRENT_SOURCE_DIR}/external/CMSIS_5/CMSIS/Core/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/FreeRTOS/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/FreeRTOS/portable/GCC/ARM_CM4F
)

# Set sources for external libraries (only for external_libs)
target_sources(external_libs INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/tusb.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/device/usbd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/device/usbd_control.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/class/audio/audio_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/class/midi/midi_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/class/cdc/cdc_device.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/common/tusb_fifo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/hw/bsp/nrf/family.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/portable/nordic/nrf5x/dcd_nrf5x.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/drivers/src/nrfx_uarte.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/drivers/src/nrfx_power.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/drivers/src/nrfx_gpiote.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/drivers/src/nrfx_spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/drivers/src/nrfx_spim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/helpers/nrfx_flag32_allocator.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/mdk/gcc_startup_${MCU_VARIANT}.S
    ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/mdk/system_${MCU_VARIANT}.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/FreeRTOS/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/FreeRTOS/queue.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/FreeRTOS/list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/FreeRTOS/timers.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/FreeRTOS/portable/GCC/ARM_CM4F/port.c
)

# Suppress specific warnings for external libraries
target_compile_options(external_libs INTERFACE
    -Wno-unused-parameter
    -Wno-unused-variable
)

# sources for the main project
target_sources(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/arcane.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bsp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cdc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/freertos_hook.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tasks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/usb_descriptors.c
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/mcp2515/src/mcp_can.c
)

# includes for the main project
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/FreeRTOSConfig
    ${CMAKE_CURRENT_SOURCE_DIR}/include/nrfx
    ${CMAKE_CURRENT_SOURCE_DIR}/include/tinyUSB
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers/mcp2515/include
)

# Link external libraries to the main project
target_link_libraries(${PROJECT_NAME} PRIVATE external_libs)



# Convert to uf2
find_program(OBJCOPY arm-none-eabi-objcopy)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.bin
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/external/uf2/utils/uf2conv.py -f 0xADA52840 --base 0x26000 ${PROJECT_NAME}.bin -c -o ${PROJECT_NAME}.uf2
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Converting ELF to UF2"
)

find_program(GDB gdb-multiarch)
# udev rules need to be set for this https://github.com/blackmagic-debug/blackmagic/tree/main/driver
set(BMP "/dev/ttyBmpGdb")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Define the 'debug-bmp' target
    add_custom_target(debug-bmp
        COMMAND ${GDB} -ex "target extended-remote ${BMP}" -ex "monitor swdp_scan" -ex "attach 1" $<TARGET_FILE:${PROJECT_NAME}>
        DEPENDS ${PROJECT_NAME}
        COMMENT "Debugging ${PROJECT_NAME} on ${BMP}"
    )
else()
    # Define the 'flash-bmp' target
    add_custom_target(flash-bmp
        COMMAND ${GDB} --batch -ex "target extended-remote ${BMP}" -ex "monitor swdp_scan" -ex "attach 1" -ex "load" $<TARGET_FILE:${PROJECT_NAME}>
        DEPENDS ${PROJECT_NAME}
        COMMENT "Flashing ${PROJECT_NAME} to ${BMP}"
    )
endif()
