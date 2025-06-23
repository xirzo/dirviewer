add_library(dir STATIC dir.c)

add_library(libdv STATIC dv.c)
target_include_directories(libdv PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(libdv PUBLIC dir)

