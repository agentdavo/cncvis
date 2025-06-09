#TinyGL Developer Guide

This document explains how the TinyGL headers and sources are organised to keep
    a clear separation of concerns.The `gl
        .h` header now focuses on standard OpenGL 1.2.1 definitions while TinyGL
            specific helpers live in `tinygl_ext.h`
        .

            ***gl.h ** – contains the portable OpenGL API constants,
    types and prototypes.Only standard functionality is declared here
        .***tinygl_ext
        .h ** – declares the optional TinyGL helper functions such
            as `glDrawText`
        .Projects include this header when they require these extensions.***src/** – implementation files mirror the functional areas of the API.  For example
  `texture.c` implements texture calls while `zbuffer.c` handles the framebuffer logic.

When adding new features follow these guidelines:
1. Group related declarations and constants together.
2. Keep TinyGL specific additions separate from standard OpenGL types.
3. Provide a matching `.c` file for each header so implementation details remain modular.

The goal is a codebase that remains easy to navigate and maintain.
