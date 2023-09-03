add_library(liboled INTERFACE)

target_sources(liboled INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/BitBang_I2C.c
  ${CMAKE_CURRENT_LIST_DIR}/ss_oled.c
  ${CMAKE_CURRENT_LIST_DIR}/ss_oled.cpp
)

target_include_directories(liboled INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

# Pull in pico libraries that we need
target_link_libraries(liboled INTERFACE pico_stdlib hardware_i2c)