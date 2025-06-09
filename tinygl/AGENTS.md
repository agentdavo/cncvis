When working in the tinygl directory:
1. Follow the refactoring progress documented in ../../REFACTOR.md. Keep that file updated when you make changes related to the 18‑step plan.
2. Run clang-format -i on any modified .c or .h files.
3. Verify the library still builds with CMake:
   ```bash
   cmake -S .. -B build
   cmake --build build
   ```