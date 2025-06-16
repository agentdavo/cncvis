cmake_minimum_required(VERSION 3.12)
project(tinygl
  DESCRIPTION "tinygl: The ultimate portable graphics library"
  HOMEPAGE_URL "https://github.com/C-Chads/tinygl"
  LANGUAGES C
  )

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")

# Set relative output directory paths, if not already defined
if (NOT BIN_DIR)
  set(BIN_DIR bin)
endif (NOT BIN_DIR)
if (NOT INCLUDE_DIR)
  set(INCLUDE_DIR include)
endif (NOT INCLUDE_DIR)
if (NOT DEFINED LIB_DIR)
  set(LIB_DIR lib)
endif (NOT DEFINED LIB_DIR)

# Check for a math library
include(CheckLibraryExists)
check_library_exists(m cos "" HAVE_M_LIBRARY)
if (HAVE_M_LIBRARY)
  set(M_LIBRARY m)
endif (HAVE_M_LIBRARY)

# Options
option(TINYGL_BUILD_EXAMPLES "Build Examples" ON)
option(TINYGL_BUILD_SHARED "Build Shared Library" ON)
option(TINYGL_BUILD_STATIC "Build Static Library" ON)
option(TINYGL_BUILD_DEBUG "Build Debug Library" OFF)
option(TINYGL_ENABLE_THREADS "Enable worker thread optimizations" ON)
set(TINYGL_NUM_THREADS "4" CACHE STRING "Number of worker threads")
option(TINYGL_ENABLE_PROFILING "Enable TinyGL function profiling" OFF)
option(TINYGL_WITH_LVGL "Install headers for use with LVGL" OFF)

if(TINYGL_WITH_LVGL)
  set(INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/tinygl)
else()
  set(INSTALL_INCLUDE_DIR ${INCLUDE_DIR})
endif()

# Build main library
add_subdirectory(src)

# Install logic for headers
add_subdirectory(include)

# Examples
if(TINYGL_BUILD_EXAMPLES)

  enable_testing()

  # These deliberately do not depend on SDL.  TODO - these could be used to
  # drive a "make test" system to check the correct functioning of the library
  # after compilation...
  add_subdirectory(Raw_Demos)

  # SDL examples were removed; only raw demos remain

endif()

# Unit tests
add_subdirectory(tests)

# Local Variables:
# tab-width: 8
# mode: cmake
# indent-tabs-mode: t
# End:
# ex: shiftwidth=2 tabstop=8
