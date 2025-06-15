#TinyGL OpenGL 1.2 Coverage
TinyGL fully implements the OpenGL 1.2 core specification.

The following core features are implemented:

- Vertex arrays and primitive rendering (glBegin/glEnd, glArrayElement)
- Display lists and selection mode
- Depth testing, color masking and slope-aware polygon offset
- Basic blending, alpha testing and fog controls (glFogf, glFogi, glFogfv,
  glFogiv, glAlphaFunc)
- Texture objects (`glTexImage`, `glBindTexture`, `glCopyTexImage`)
- BGR/BGRA pixel formats for textures and readback
- Optional KTX loader to import textures and cube maps
- `glDrawRangeElements` for indexed vertex drawing
- `glDrawElements` convenience wrapper for indexed drawing
- `glDepthFunc` selects depth test comparison
- `glLineWidth` updates line raster width (unused)
- Stubs for 3D texture entry points
- Matrix stack and lighting operations
- Client-side attribute enabling/disabling
- Basic scissor testing via `glScissor` and `GL_SCISSOR_TEST`
- Polygon offset adjustment and stencil/logic op stubs
- Textured primitives including per-vertex colors
- `glDrawArrays` for array-based rendering
- `glDrawPixels` and `glPixelZoom` for software image blits
- `glGetIntegerv` and `glGetFloatv` expose basic state
- `glTexEnvi` and `glTexParameteri` store texture env and filter settings
- Edge-clamp texture wrapping (`GL_CLAMP` and `GL_CLAMP_TO_EDGE`)
- `glTexEnvf`/`glTexParameterf` accept floating point values
- `glPixelStorei` configures pack and unpack alignment
- `glReadPixels` retrieves framebuffer contents
- `glTexSubImage1D`/`glTexSubImage2D` update texture regions
- `glCopyTexSubImage2D` copies framebuffer data to textures
- `glIsEnabled` queries current enable state
- Raster position helpers (`glRasterPos*`) and `glRectf`
- `glTexImage1D` (internally resized to 2D)
- `glGetString` returns vendor/renderer/version strings
- Display lists detect deep recursion (limit 32)
- Benchmark utility exercises many GL 1.1 calls with a rotating icosahedron

Alpha testing and stencil test state are handled through `glAlphaFunc`
and `glStencilFunc` along with the standard enable flags. Accumulation
buffer calls are supported.
