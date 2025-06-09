# Common OpenGL 1.1 Pitfalls

This document summarises mistakes that often lead to subtle bugs or poor performance. TinyGL now avoids several of these issues out of the box.

1. **Improperly Scaled Normals** – lighting requires unit-length normals. TinyGL enables `GL_NORMALIZE` during context initialisation so scaling matrices do not break lighting.
2. **Poor Tessellation Hurts Lighting** – very coarse meshes limit per-vertex lighting accuracy. Ensure models have enough vertices to capture highlights.
3. **Remember Your Matrix Mode** – always set the matrix mode before modifying matrices; TinyGL assumes `GL_MODELVIEW` by default in callbacks.
4. **Projection Stack Overflow** – TinyGL guarantees at least a two-level projection stack; avoid deep pushes when drawing overlays.
5. **Incomplete Mipmaps** – `glTexImage2D` calls within TinyGL build all levels so mipmapped filters work correctly.
6. **Reading Back Luminance Pixels** – colour conversion to luminance follows the NTSC weights.
7. **Pixel Store Alignment** – row alignment defaults to 4 in OpenGL; TinyGL packs rows tightly and assumes alignment of 1 when copying pixels.
8. **Saving and Restoring Pixel Store State** – TinyGL’s texture routines restore state they modify.
9. **Raster Position Clipping** – the raster position must lie inside the view frustum. Use `glWindowPos*` helpers to set pixel positions reliably.
10. **Viewport Does Not Clip** – the viewport only transforms coordinates; use scissor tests if clipping is required.
11. **Setting the Raster Colour** – colour changes do not affect the raster position until after calling `glRasterPos*`.
12. **Lower-Left Origin** – TinyGL follows OpenGL’s lower-left origin. Images loaded from files may need to be flipped in Y.
13. **Pixel-Exact Raster Positions** – `glWindowPos`-like helpers are recommended when drawing to specific screen pixels.
14. **Colour Material Order** – always call `glColorMaterial` before enabling `GL_COLOR_MATERIAL`.
15. **Shared Raster State** – per-fragment options (depth test, blending, etc.) affect bitmaps and pixel copies too; TinyGL resets these around internal draws.
16. **Allocate Needed Buffers** – ensure your context requests depth/stencil buffers when required.
