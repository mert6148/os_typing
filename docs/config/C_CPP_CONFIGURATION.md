# C/C++ Configuration and Build Guide

## Compiler Selection

### GCC
```bash
# Install
sudo apt-get install build-essential gcc g++

# Version check
gcc --version
g++ --version

# Compile C file
gcc -Wall -Wextra -std=c11 -O2 src/os_checking.c -o os_checking

# Compile C++ file
g++ -Wall -Wextra -std=c++17 -O2 scripts/c-c++/os_controlsystem.cpp -o os_controlsystem
```

### Clang
```bash
# Install
sudo apt-get install clang

# Version check
clang --version
clang++ --version

# Compile
clang -Wall -Wextra -std=c11 -O2 src/os_checking.c -o os_checking
clang++ -Wall -Wextra -std=c++17 -O2 scripts/c-c++/os_controlsystem.cpp -o os_controlsystem
```

### MSVC (Windows)
```powershell
# Install Visual Studio Build Tools
# Then use MSVC compiler

cl /W4 /O2 /std:c11 src/os_checking.c
cl /W4 /O2 /std:c++17 scripts/c-c++/os_controlsystem.cpp
```

## Makefile Configuration

### Basic Makefile
```makefile
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -std=c11 -O2
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

all: os_checking os_controlsystem

os_checking: src/os_checking.c
	$(CC) $(CFLAGS) -o $@ $<

os_controlsystem: scripts/c-c++/os_controlsystem.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

test:
	./os_checking --test
	./os_controlsystem --test

clean:
	rm -f os_checking os_controlsystem

.PHONY: all test clean
```

### Running Make
```bash
# Build all targets
make

# Build specific target
make os_checking

# Run tests
make test

# Clean build artifacts
make clean
```

## CMake Configuration

### CMakeLists.txt Structure
```cmake
cmake_minimum_required(VERSION 3.10)
project(OsTyping C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Add library
add_library(ostying src/os_checking.c src/terminal.c)

# Add executable
add_executable(os_typing_app src/main.c)
target_link_libraries(os_typing_app ostying)

# Enable testing
enable_testing()
add_subdirectory(tests)
```

### Building with CMake
```bash
# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build
cmake --build .

# Run tests
ctest

# Install
cmake --install .

# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

## Compiler Flags

### Optimization Flags
```bash
-O0     # No optimization (debug)
-O1     # Basic optimization
-O2     # Standard optimization
-O3     # Aggressive optimization
-Os     # Optimize for size
-Ofast  # Optimize for speed (unsafe)
```

### Warning Flags
```bash
-Wall           # Most common warnings
-Wextra         # Extra warnings
-Wpedantic      # Strict ISO compliance
-Werror         # Treat warnings as errors
-Wstrict-prototypes  # Require function prototypes (C only)
```

### Debug Flags
```bash
-g      # Generate debug symbols
-DDEBUG # Define DEBUG macro
-fno-omit-frame-pointer  # Better stack traces
```

### Security Flags
```bash
-fstack-protector-strong     # Stack overflow protection
-D_FORTIFY_SOURCE=2          # Buffer overflow checks
-fPIE -pie                   # Position Independent Executable
```

## Static Analysis

### GCC Warnings
```bash
gcc -Wall -Wextra -Wpedantic -Wconversion src/os_checking.c
```

### Clang Static Analyzer
```bash
clang --analyze src/*.c
scan-build gcc -Wall src/*.c
```

### Lint Tools
```bash
# Install cpplint
pip install cpplint

# Run cpplint
cpplint --recursive src/
```

## Memory Checking

### Valgrind
```bash
# Compile with debug symbols
gcc -g -Wall src/os_checking.c -o os_checking

# Run with Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./os_checking

# Detailed memory trace
valgrind --leak-check=full --track-origins=yes ./os_checking
```

### AddressSanitizer
```bash
# Compile with ASan
gcc -fsanitize=address -g -Wall src/os_checking.c -o os_checking

# Run
./os_checking
# ASAN_OPTIONS=verbosity=1:halt_on_error=1 ./os_checking
```

### UndefinedBehaviorSanitizer
```bash
# Compile with UBSan
gcc -fsanitize=undefined -g -Wall src/os_checking.c -o os_checking

# Run
./os_checking
```

## Unit Testing

### CUnit Framework
```c
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_os_detection() {
    int result = get_os_type();
    CU_ASSERT(result >= 0);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("OS Tests", NULL, NULL);
    CU_add_test(suite, "OS Detection", test_os_detection);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
```

### Google Test (C++)
```cpp
#include <gtest/gtest.h>

TEST(OsCheckingTest, DetectOS) {
    int result = get_os_type();
    EXPECT_GE(result, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

## Compiler Optimization Tips

1. Profile before optimizing
2. Use -O2 for balanced performance
3. Enable LTO for better optimization: `-flto`
4. Use profile-guided optimization: `-fprofile-generate` and `-fprofile-use`
5. Consider architecture-specific flags: `-march=native`
6. Don't use `-Ofast` in production (unsafe)
7. Test thoroughly after optimization

## Best Practices

- Always use `-Wall -Wextra` for compilation
- Enable debug symbols in development: `-g`
- Use address sanitizer to catch memory errors: `-fsanitize=address`
- Keep object files in separate directory (build/)
- Use version control for all code
- Document build process in README
- Use CI/CD to test on multiple platforms
- Profile memory usage regularly
- Test with various compiler versions
