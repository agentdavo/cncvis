add_executable(tgl_unit_basic basic.c)
target_include_directories(tgl_unit_basic PRIVATE ../include ../src)
target_link_libraries(tgl_unit_basic tinygl ${M_LIBRARY})
add_test(NAME tinygl_basic COMMAND tgl_unit_basic)
