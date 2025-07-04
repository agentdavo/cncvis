# cncvis/CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(cncvis LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# =============================================================================
#                                DIRECTORIES
# =============================================================================

# Define directories relative to the `cncvis` directory
set(TGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tinygl")
set(STLIO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libstlio")
set(STB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/stb")
set(MXML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mxml")

# Define the static library for cncvis
add_library(cncvis STATIC
    utils.c
    actor.c
    assembly.c
    camera.c
    light.c
    config.c
    api.c
    osd.c
)

# Include directories for cncvis
target_include_directories(cncvis PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${TGL_DIR}/include
    ${TGL_DIR}/include-demo
    ${STLIO_DIR}/include
    ${STB_DIR}
    ${MXML_DIR}
)

# =============================================================================
#                             SUBDIRECTORIES
# =============================================================================

# Add TinyGL subdirectory
set(TINYGL_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(TINYGL_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(tinygl)

# Add STLIO subdirectory
add_subdirectory(libstlio)

# Add STB subdirectory (assuming it's header-only)
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${STB_DIR}")

# Add mxml subdirectory
add_subdirectory(mxml)

# =============================================================================
#                              LINKING DEPENDENCIES
# =============================================================================

# Link dependencies to cncvis
target_link_libraries(cncvis PRIVATE
    tinygl
    stlio
    stb
    mxml_static # Use mxml_static instead of mxml
    m  # Link against the math library
)

# =============================================================================
#                               INSTALLATION
# =============================================================================

# Define installation rules for cncvis library
install(TARGETS cncvis
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cncvis"
)

# ================================
#                  TESTS
# ================================
enable_testing()

# Copy machine descriptions to build directory so tests can access them
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/machines
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(cncvis_test cncvis_test.c)
target_include_directories(cncvis_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${TGL_DIR}/include
    ${TGL_DIR}/include-demo
    ${STLIO_DIR}/include
    ${STB_DIR}
    ${MXML_DIR}
)
target_link_libraries(cncvis_test PRIVATE cncvis tinygl stlio stb mxml_static m)

add_test(NAME cncvis_basic COMMAND cncvis_test)