cmake_minimum_required(VERSION 3.12)

# Change your executable name to something creative!
set(NAME PICOKEY) # <-- Name your project/executable here!
set(ENV{FREERTOS_KERNEL_PATH}  ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS-Kernel)
# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(${NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(FAMILY rp2040)
set(PICO_BOARD pico_w) # Set the board used
set(BOARD pico_sdk)
add_subdirectory(pico-cpp)
add_subdirectory(lib)

# set(FREERTOS_PORT GCC_RP2040 STRING)
# Initialize all other external libs & dependencies
include(FreeRTOS_Kernel_import.cmake)
# Initialize the SDK
pico_sdk_init()

add_subdirectory(src)


# Set config files paths to be used
include(lwip_import.cmake) # lwip config files
set(FREERTOS_CONFIG_FILE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/configs/FreeRTOS-Kernel" CACHE STRING "Local Config")
include_directories("${FREERTOS_CONFIG_FILE_DIRECTORY}") # FreeRTOS config files
# Make sure TinyUSB can find tusb_config.h
include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${FREERSTOS_KERNEL_PATH}/include)




