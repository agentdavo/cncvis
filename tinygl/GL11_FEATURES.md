# TinyGL OpenGL 1.1 Coverage

The following core features are implemented:

- Vertex arrays and primitive rendering (glBegin/glEnd, glArrayElement)
- Display lists and selection mode
- Depth testing, color masking and slope-aware polygon offset
- Basic blending, alpha testing and fog controls (glFogf, glFogi, glFogfv,
  glFogiv, glAlphaFunc)
- Texture objects (`glTexImage`, `glBindTexture`, `glCopyTexImage`)
- Matrix stack and lighting operations
- Client-side attribute enabling/disabling
- Basic scissor testing via `glScissor` and `GL_SCISSOR_TEST`
- Polygon offset adjustment and stencil/logic op stubs
- Textured primitives including per-vertex colors
- `glDrawArrays` for array-based rendering
- `glDrawPixels` and `glPixelZoom` for software image blits
- Raster position helpers (`glRasterPos*`) and `glRectf`
- `glTexImage1D` (internally resized to 2D)
- `glGetString` returns vendor/renderer/version strings
- Benchmark utility exercises many GL 1.1 calls with a rotating icosahedron

Alpha testing and stencil test state are tracked with `glAlphaFunc`,
`glStencilFunc`, and related enables, though no pixels are currently
discarded since TinyGL lacks an alpha or stencil buffer. Accumulation
buffer calls remain stubs.

Further unsupported or partial features are described in `LIMITATIONS`.
