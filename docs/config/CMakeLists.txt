cmake_minimum_required(VERSION 3.10)
project(OsTyping C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler flags
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    
    # Debug configuration
    set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -DDEBUG")
    
    # Release configuration
    set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
elseif(MSVC)
    add_compile_options(/W4)
    set(CMAKE_C_FLAGS_DEBUG "/Zi /Od /D_DEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od /D_DEBUG")
    set(CMAKE_C_FLAGS_RELEASE "/O2 /DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
endif()

# Source files
file(GLOB_RECURSE SOURCES "src/*.c" "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

# Create library
add_library(ostying ${SOURCES})
target_include_directories(ostying PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Create executable
add_executable(os_typing_app src/main.c)
target_link_libraries(os_typing_app ostying)

# Enable testing
enable_testing()

# Add subdirectories
add_subdirectory(tests)

# Doxygen documentation
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/docs")
    doxygen_add_docs(docs ${SOURCES} ${HEADERS})
endif()

# Installation
install(TARGETS os_typing_app DESTINATION bin)
install(DIRECTORY src/ DESTINATION include/ostying FILES_MATCHING PATTERN "*.h")
