include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(tinygl_srcs
  accum.c
  api.c
  state.c
  arrays.c
  clear.c
  clip.c
  misc.c
  utils.c
  init.c
  lighting.c
  list.c
  matrix.c
  memory.c
  lockstepthread.c
  selection.c
  texture.c
  vertex.c
  zbuffer.c
  zmath.c
  raster.c
  extensions.c
  evaluators.c
  fog.c
  )

find_package(Threads REQUIRED)

if(TINYGL_BUILD_SHARED)
  add_library(tinygl SHARED ${tinygl_srcs})
  target_include_directories(tinygl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
  install(TARGETS tinygl
    RUNTIME DESTINATION ${BIN_DIR}
    LIBRARY DESTINATION ${LIB_DIR}
    ARCHIVE DESTINATION ${LIB_DIR})
  if(NOT MSVC)
    target_compile_options(tinygl PRIVATE -O3 -DNDEBUG -pedantic -Wall -Wno-unused-function)
    if(NOT CMAKE_CROSSCOMPILING)
      target_compile_options(tinygl PRIVATE -march=x86-64 -mtune=generic)
    endif()
  endif(NOT MSVC)
  target_link_libraries(tinygl PRIVATE Threads::Threads)
endif(TINYGL_BUILD_SHARED)

if(TINYGL_BUILD_STATIC)
  add_library(tinygl-static STATIC ${tinygl_srcs})
  target_include_directories(tinygl-static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
  install(TARGETS tinygl-static
    RUNTIME DESTINATION ${BIN_DIR}
    LIBRARY DESTINATION ${LIB_DIR}
    ARCHIVE DESTINATION ${LIB_DIR})
  if(NOT MSVC)
    target_compile_options(tinygl-static PRIVATE -O3 -DNDEBUG -pedantic -Wall -Wno-unused-function)
    if(NOT CMAKE_CROSSCOMPILING)
      target_compile_options(tinygl-static PRIVATE -march=x86-64 -mtune=generic)
    endif()
  endif(NOT MSVC)
  target_link_libraries(tinygl-static PRIVATE Threads::Threads)
endif(TINYGL_BUILD_STATIC)

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

