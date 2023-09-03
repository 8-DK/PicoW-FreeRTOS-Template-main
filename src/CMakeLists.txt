include_directories(${NAME}  PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include ${FREERSTOS_KERNEL_PATH}/include)

add_executable(${NAME}  
		main.cpp
		mainApp.cpp 
		ledHelper.cpp 
		usbHelper.cpp 
		keyHelper.cpp 
                displayHelper.cpp
		utils.cpp 
                usb_descriptors.c
                msc_disk_dual.c
        )

# pull in common dependencies
target_link_libraries(${NAME} 
        pico_stdlib
        pico_cyw43_arch_lwip_threadsafe_background
        FreeRTOS-Kernel-Heap4
        hardware_i2c
        hardware_adc
        LWIP_PORT
        FreeRTOS-Kernel
        FreeRTOS-Kernel-Heap4
        hardware_dma 		
        hardware_flash 
        tinyusb_device 
        tinyusb_board 
        pico-cpp 
        hardware_spi 
        hardware_i2c 
        libeeprom 
        liboled 
        libcJson 
        libspistorage
        libLCD1in3
        )

# create map/bin/hex file etc.
# Enable print functionality on usb & disable on uart
pico_enable_stdio_usb(${NAME}  1) 
pico_enable_stdio_uart(${NAME}  0)
pico_add_extra_outputs(${NAME})
include(${CMAKE_CURRENT_LIST_DIR}/../lib/ws2812Lib/CMakeLists.txt)
linkws2812lib(${NAME} ${CMAKE_CURRENT_LIST_DIR}/../lib/ws2812Lib)	

target_include_directories(${NAME}  
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../configs/FreeRTOS-Kernel 
        # PUBLIC ../FreeRTOS-Kernel/include 
        PUBLIC ../include 
        # PUBLIC ../FreeRTOS-Kernel/portable/GCC/ARM_CM0
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}
        PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../include
        PUBLIC ../local-libs/ssd1306 # SSD1306 OLED DISPLAY LOCAL LIBRARY
        PUBLIC ../local-libs/am2320 # AM2320 SENSOR LOCAL LIBRARY
        PUBLIC ../local-libs/helpers # ALL HELPER LOCAL LIBRARIES
        # PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include ${FREERSTOS_KERNEL_PATH}/include
        )
