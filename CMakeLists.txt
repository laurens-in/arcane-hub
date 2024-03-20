cmake_minimum_required(VERSION 3.27.4)

project(
	arcane
	VERSION 1.0
	LANGUAGES C ASM
)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")

set(MCU_VARIANT nrf52840)
set(LD_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linker/${MCU_VARIANT}.ld)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain/arm_gcc.cmake)
set(CMAKE_EXECUTABLE_SUFFIX .elf)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4 CACHE INTERNAL "System Processor")

add_executable(${PROJECT_NAME})

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

# sources
target_sources(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/usb_descriptors.c
        ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/tusb.c
        ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/device/usbd.c
        ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/device/usbd_control.c
        ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/class/audio/audio_device.c
        ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/class/midi/midi_device.c
        ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/common/tusb_fifo.c
        ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/hw/bsp/nrf/family.c
        ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src/portable/nordic/nrf5x/dcd_nrf5x.c
        ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/drivers/src/nrfx_uarte.c
        ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/drivers/src/nrfx_power.c
        ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/mdk/gcc_startup_${MCU_VARIANT}.S
        ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/mdk/system_${MCU_VARIANT}.c
)

# includes
target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/hw
        ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/hw/bsp/nrf
        ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/hw/bsp/nrf/boards/feather_nrf52840_express
        ${CMAKE_CURRENT_SOURCE_DIR}/external/tinyUSB/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx
        ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/mdk
        ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/drivers/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/drivers/src
        ${CMAKE_CURRENT_SOURCE_DIR}/external/nrfx/hal
        ${CMAKE_CURRENT_SOURCE_DIR}/external/CMSIS_5/CMSIS/Core/Include
)

# TODO: remove and fix in toolchain
set(CMAKE_C_FLAGS
        "-mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -g"
)

# Convert to uf2
set(CMAKE_OBJCOPY /usr/lib/arm-none-eabi/bin/objcopy)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.bin
        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/external/uf2/utils/uf2conv.py ${PROJECT_NAME}.bin -c -o ${PROJECT_NAME}.uf2
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Converting ELF to UF2"
)


# set(FREERTOS_KERNEL_PATH external/FreeRTOS)
# set(FREERTOS_PORT GCC_ARM_CM4F CACHE STRING "")

# # Add the freertos_config for FreeRTOS-Kernel
# add_library(freertos_config INTERFACE)

# target_include_directories(freertos_config
#     INTERFACE
#     src/FreeRTOSConfig
# )

# add_subdirectory(${FREERTOS_KERNEL_PATH})


# TinyUSB
# add_library(tinyusb_config INTERFACE)
# target_include_directories(tinyusb_config
#   INTERFACE
#   src/
# )

# target_include_directories(${PROJECT_NAME}
#   PRIVATE
#   external/tinyUSB/hw
#   external/tinyUSB/src
# )

# add_subdirectory(external/tinyUSB/src)
