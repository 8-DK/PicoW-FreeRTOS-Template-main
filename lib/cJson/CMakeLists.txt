add_library(libcJson cJSON.c cJSON.h cJSON_Utils.c cJSON_Utils.h)
target_include_directories(libcJson PUBLIC .)
target_link_libraries(libcJson PUBLIC pico_stdlib pico-cpp)