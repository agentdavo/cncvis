add_executable(tgl_unit_basic basic.c)
target_include_directories(tgl_unit_basic PRIVATE ../include ../src)
target_link_libraries(tgl_unit_basic tinygl ${M_LIBRARY})
add_test(NAME tinygl_basic COMMAND tgl_unit_basic)

add_executable(tgl_unit_colored colored.c)
target_include_directories(tgl_unit_colored PRIVATE ../include ../src)
target_link_libraries(tgl_unit_colored tinygl ${M_LIBRARY})
add_test(NAME tinygl_colored COMMAND tgl_unit_colored)

add_executable(tgl_unit_textured textured.c)
target_include_directories(tgl_unit_textured PRIVATE ../include ../src)
target_link_libraries(tgl_unit_textured tinygl ${M_LIBRARY})
add_test(NAME tinygl_textured COMMAND tgl_unit_textured)
