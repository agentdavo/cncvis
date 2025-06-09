When modifying code in this tinygl folder:

1. Format all `.c` and `.h` files using `clang-format -i` before committing.
2. Ensure the project still builds using CMake:
   ```bash
   cmake -S . -B build
   cmake --build build
   ./build/tinygl/Raw_Demos/raw_benchmark 10
   ```
3. Update README.md in this folder if the public API or build steps change