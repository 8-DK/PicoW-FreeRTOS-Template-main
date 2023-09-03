add_library(libeeprom EEPROM.cpp EEPROM.h)
target_include_directories(libeeprom PUBLIC .)
target_link_libraries(libeeprom PUBLIC pico_stdlib pico-cpp hardware_flash)