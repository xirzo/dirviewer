add_library(libdv STATIC dv.c)

target_include_directories(libdv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
