# cncvis

Lightweight CNC machine visualization library built around TinyGL. It loads machine descriptions from XML, renders STL models and exposes a small camera and lighting API.

## Directory Layout
- **tinygl/** – software OpenGL implementation
- **libstlio/** – simple STL loader
- **mxml/** – Mini-XML parser
- **stb/** – minimal subset of stb headers
- **machines/** – example machine configurations

## Building
1. Configure with CMake
   ```bash
   cmake -S cncvis -B build
   cmake --build build
   ```
2. Optional settings
   - `-DTINYGL_BUILD_DEBUG=ON` builds an unoptimised TinyGL library
   - `-DTINYGL_ENABLE_THREADS=OFF` disables the helper worker thread
   - `-DTINYGL_NUM_THREADS=<n>` controls worker count
   - `-DTINYGL_WITH_LVGL=ON` builds LVGL helpers including `tgl_to_lvgl()`

The build produces `libcncvis.a` and several TinyGL demos under `build/`.

## LVGL Integration

Compile TinyGL with LVGL support enabled:

```bash
cmake -S cncvis -B build -DTINYGL_WITH_LVGL=ON
cmake --build build
```

In your LVGL display driver's flush callback, call `tgl_to_lvgl()` after rendering
to copy the TinyGL framebuffer into the LVGL draw buffer.  Link your application
against the TinyGL library produced in `build/lib`.

TinyGL's LVGL bridge works with 16‑bit RGB565 and 32‑bit ARGB8888 color buffers.
Other formats are unsupported.

## Tests
Run the suite with:
```bash
ctest --test-dir build --output-on-failure
```
Unit tests cover common OpenGL calls, BGR uploads, draw range/element helpers, and basic rendering benchmarks.

## Recent Changes
- BGR/BGRA texture upload and readback
- `glDrawRangeElements`, `glDrawElements` and depth function support
- Optional lock-step worker thread controlled by `TINYGL_ENABLE_THREADS`
- KTX texture loader for loading compressed assets
- Additional unit tests including a comprehensive GL feature check
- Partial OpenGL 1.2 core coverage. See `tinygl/GL12_FEATURES.md` for details

## License
MIT. See `LICENSE` for details.
