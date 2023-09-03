function(linkws2812lib TARGET EXPATH)
        file(MAKE_DIRECTORY ${EXPATH}/generated)

        # generate the header file into the source tree as it is included in the RP2040 datasheet
        pico_generate_pio_header(${TARGET} ${EXPATH}/ws2812.pio OUTPUT_DIR ${EXPATH}/generated)

        target_include_directories(${TARGET} PUBLIC ${EXPATH} ${EXPATH}/generated)

        target_sources(${TARGET} PRIVATE ${EXPATH}/NeoPixelConnect.cpp ${EXPATH}/NeoPixelConnect.h ${EXPATH}/NeoPixelConnectPara.cpp ${EXPATH}/NeoPixelConnectPara.h)

        # target_link_libraries(${TARGET} PRIVATE pico_stdlib hardware_pio)
        # pico_add_extra_outputs(${TARGET})

        # Additionally generate python and hex pioasm outputs for inclusion in the RP2040 datasheet
        add_custom_target(pio_ws2812_datasheet DEPENDS ${EXPATH}/generated/ws2812.py)
        add_custom_command(OUTPUT ${EXPATH}/generated/ws2812.py
                DEPENDS ${EXPATH}/ws2812.pio
                COMMAND Pioasm -o python ${EXPATH}/ws2812.pio ${EXPATH}/generated/ws2812.py
                VERBATIM)
        add_dependencies(${TARGET} pio_ws2812_datasheet)
endfunction()