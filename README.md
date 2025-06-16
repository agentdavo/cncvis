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

## Configuration
Assemblies may define motion limits inside the `<limits>` element:

```xml
<limits>
  <linear min="0" max="100"/>
  <rotary min="-90" max="90"/>
</limits>
```

When a limit is exceeded by more than 1 unit, `ucncUpdateMotion` returns `-1`
and the assembly's `limitTriggered` flag becomes `1`. Use
`ucncClearLimitWarning("assembly")` to reset this state.

## Recent Changes
- BGR/BGRA texture upload and readback
- `glDrawRangeElements`, `glDrawElements` and depth function support
- Optional lock-step worker thread controlled by `TINYGL_ENABLE_THREADS`
- KTX texture loader for loading compressed assets
- Additional unit tests including a comprehensive GL feature check
- Full OpenGL 1.2 core compliance
- Motion limit enforcement and `ucncClearLimitWarning` API

## License
MIT. See `LICENSE` for details.
