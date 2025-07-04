#TinyGL Refactor Progress

This document tracks progress of the 18-step refactoring plan. The goal is to split TinyGL into modules that mirror the functional groups in `gl.h`.

| Step | Description | Status |
|-----|-------------|--------|
|1|Analyze existing codebase and generate `function_map.json`|**Completed**|
|2|Create `internal.h` with context helpers|**Completed**|
|3|Create module headers (`gl_state.h`, `gl_vertex.h`, ...)|**Completed**|
|4|Move state management to `state.c`|**Completed**|
|5|Move vertex specification to `vertex.c`|**Completed**|
|6|Create raster module (`gl_raster.h`, `raster.c`) and migrate raster functions|**Completed** (zline/ztriangle included)|
|7|Refactor `texture.c` and remove `image_util.c`|**Completed**|
|8|Refactor `lighting.c` and remove `specbuf.c`|**Completed**|
|9|Create `evaluators.c` for NURBS|**Completed** (stubbed)|
|10|Implement fog in `fog.c`|**Completed**|
|11|Rename `select.c` to `selection.c` and update logic|**Completed**|
|12|Create `extensions.c` for TinyGL extensions|**Completed**|
|13|Refactor initialization in `init.c`|**Completed**|
|14|Move utilities to `utils.c`|**Completed**|
|15|Update `zbuffer.c`, `matrix.c`, `memory.c`, `zmath.c`|**Completed**|
|16|Update build system for new modules|**Completed**|
|17|Add and run unit tests|Completed (validated under Valgrind with colored and textured rendering tests)|
|18|Optimize for software rendering|Completed|
|19|Always use multithreaded paths, remove single-thread code|Completed|
|20|Merge `chad_math.h` into `zmath` for SIMD-friendly math|Completed|
|21|Add cube benchmark test comparing threading performance|Completed|
|22|Implement optional KTX texture loader|Completed|
|23|Add red KTX sample texture used by gears test|Completed|
|24|Achieve full OpenGL 1.2 compliance|Completed|
|25|Add option to disable TinyGL tests (`TINYGL_BUILD_TESTS`)|Completed|
