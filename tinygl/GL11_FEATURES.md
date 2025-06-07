# TinyGL OpenGL 1.1 Coverage

The following core features are implemented:

- Vertex arrays and primitive rendering (glBegin/glEnd, glArrayElement)
- Display lists and selection mode
- Depth testing, color masking and slope-aware polygon offset
- Basic blending and fog controls (glFogf, glFogi, glFogfv, glFogiv)
- Texture objects (`glTexImage`, `glBindTexture`, `glCopyTexImage`)
- Matrix stack and lighting operations
- Client-side attribute enabling/disabling
- Basic scissor testing via `glScissor` and `GL_SCISSOR_TEST`
- Textured primitives including per-vertex colors
- Benchmark utility exercises many GL 1.1 calls with a rotating icosahedron

The stencil and accumulation buffers are not implemented. Stub
functions (`glAccum`, `glAlphaFunc`, `glStencil*`, `glClear*`) are
provided so applications expecting the full OpenGL 1.1 API will link.

Further unsupported or partial features are described in `LIMITATIONS`.
