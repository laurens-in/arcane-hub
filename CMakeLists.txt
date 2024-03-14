cmake_minimum_required(VERSION 3.27.4)

project(
	arcane
	VERSION 1.0
	LANGUAGES C ASM
)

add_executable(${PROJECT_NAME} main.c)

set(CMAKE_C_COMPILER "arm-none-eabi-gcc")

# Add linker script
target_link_options(${PROJECT_NAME} PRIVATE -T ../layout.ld)

# Add compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall 
	-Wextra 
	-Wpedantic 
	-O3
	-nostdlib 
	-ffreestanding 
	-ffunction-sections 
	-fdata-sections
    -mthumb 
	-mcpu=cortex-m4 
	-mfloat-abi=hard 
	-mfpu=fpv4-sp-d16
)

# Set the linker flags
set(LINKER_FLAGS "-Wl,--gc-sections -lc -lnosys")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKER_FLAGS}")

# Generate UF2 file
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
    COMMAND uf2conv ${PROJECT_NAME}.bin -c -o ${PROJECT_NAME}.uf2
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Converting ELF to UF2"
)
