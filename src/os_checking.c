#include "os_checking.h"

#include <stdio.h>

/**
 * @file os_checking.c
 * @brief Implementation of portable OS detection helpers.
 * 
 * Uses compile-time preprocessor checks to determine the operating system
 * at compile time. Functions are simple, fast, and suitable for conditional
 * compilation paths and initialization routines.
 */

/**
 * @brief Implementation: is_windows()
 * Checks Windows-specific predefined macros (_WIN32, _WIN64).
 */
bool is_windows(void) {
#if defined(_WIN32) || defined(_WIN64)
    return true;
#else
    return false;
#endif
}

bool is_unix(void) {
#if defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MACH__)
    return true;
#else
    return false;
#endif
}

bool is_linux(void) {
#if defined(__linux__)
    return true;
#else
    return false;
#endif
}

bool is_macos(void) {
#if defined(__APPLE__) && defined(__MACH__)
    return true;
#else
    return false;
#endif
}

const char *os_name(void) {
#if defined(_WIN32) || defined(_WIN64)
    return "Windows";
#elif defined(__APPLE__) && defined(__MACH__)
    return "macOS";
#elif defined(__linux__)
    return "Linux";
#elif defined(__unix__) || defined(__unix)
    return "Unix";
#else
    return "Unknown";
#endif
}

/*
 * Self-test: compile with -DOS_CHECKING_TEST to build a small test program.
 * Example (Linux/MinGW):
 *   gcc -DOS_CHECKING_TEST -o os_checking os_checking.c && ./os_checking
 */
#ifdef OS_CHECKING_TEST
int main(void) {
    printf("OS detection test\n");
    printf("is_windows: %s\n", is_windows() ? "yes" : "no");
    printf("is_unix:    %s\n", is_unix() ? "yes" : "no");
    printf("is_linux:   %s\n", is_linux() ? "yes" : "no");
    printf("is_macos:   %s\n", is_macos() ? "yes" : "no");
    printf("os_name:    %s\n", os_name());
    return 0;
}
#endif
