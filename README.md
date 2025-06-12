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

The build produces `libcncvis.a` and several TinyGL demos under `build/`.

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
- Additional unit tests including a comprehensive GL feature check

## License
MIT. See `LICENSE` for details.
