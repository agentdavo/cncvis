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
  selection.c
  texture.c
  vertex.c
  zbuffer.c
  zmath.c
  raster.c
  extensions.c
  evaluators.c
  fog.c
  profiling.c
  ktx_loader.c
  )

if(TINYGL_WITH_LVGL)
  list(APPEND tinygl_srcs lvgl_bridge.c)
endif()

if(TINYGL_ENABLE_THREADS)
  list(APPEND tinygl_srcs lockstepthread.c)
endif()

if(TINYGL_ENABLE_THREADS)
  find_package(Threads REQUIRED)
endif()

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
  if(TINYGL_ENABLE_THREADS)
    target_link_libraries(tinygl PRIVATE Threads::Threads)
  endif()
  target_compile_definitions(tinygl PUBLIC TGL_ENABLE_THREADS=$<BOOL:${TINYGL_ENABLE_THREADS}>)
  target_compile_definitions(tinygl PUBLIC TGL_NUM_THREADS=${TINYGL_NUM_THREADS})
  target_compile_definitions(tinygl PUBLIC TGL_FEATURE_PROFILING=$<BOOL:${TINYGL_ENABLE_PROFILING}>)
  target_compile_definitions(tinygl PUBLIC TINYGL_WITH_LVGL=$<BOOL:${TINYGL_WITH_LVGL}>)
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
  if(TINYGL_ENABLE_THREADS)
    target_link_libraries(tinygl-static PRIVATE Threads::Threads)
  endif()
  target_compile_definitions(tinygl-static PUBLIC TGL_ENABLE_THREADS=$<BOOL:${TINYGL_ENABLE_THREADS}>)
  target_compile_definitions(tinygl-static PUBLIC TGL_NUM_THREADS=${TINYGL_NUM_THREADS})
  target_compile_definitions(tinygl-static PUBLIC TGL_FEATURE_PROFILING=$<BOOL:${TINYGL_ENABLE_PROFILING}>)
  target_compile_definitions(tinygl-static PUBLIC TINYGL_WITH_LVGL=$<BOOL:${TINYGL_WITH_LVGL}>)
endif(TINYGL_BUILD_STATIC)

if(TINYGL_BUILD_DEBUG)
  add_library(tinygl-debug STATIC ${tinygl_srcs})
  target_include_directories(tinygl-debug PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
  if(NOT MSVC)
    target_compile_options(tinygl-debug PRIVATE -O0 -g -pedantic -Wall -Wextra)
    if(NOT CMAKE_CROSSCOMPILING)
      target_compile_options(tinygl-debug PRIVATE -march=x86-64 -mtune=generic)
    endif()
  endif()
  if(TINYGL_ENABLE_THREADS)
    target_link_libraries(tinygl-debug PRIVATE Threads::Threads)
  endif()
  target_compile_definitions(tinygl-debug PUBLIC TGL_ENABLE_THREADS=$<BOOL:${TINYGL_ENABLE_THREADS}>)
  target_compile_definitions(tinygl-debug PUBLIC TGL_NUM_THREADS=${TINYGL_NUM_THREADS})
  target_compile_definitions(tinygl-debug PUBLIC TGL_FEATURE_PROFILING=$<BOOL:${TINYGL_ENABLE_PROFILING}>)
  target_compile_definitions(tinygl-debug PUBLIC TINYGL_WITH_LVGL=$<BOOL:${TINYGL_WITH_LVGL}>)
endif()

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8

