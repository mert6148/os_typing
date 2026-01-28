#ifndef OS_CHECKING_H
#define OS_CHECKING_H

/**
 * @file os_checking.h
 * @brief Portable OS detection and identification helpers.
 * 
 * Provides cross-platform detection for Windows, Unix/Linux, macOS,
 * and returns a human-readable OS name. Useful for conditional compilation
 * and runtime environment checks.
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check if the current OS is Windows.
 * @return true if running on Windows, false otherwise.
 */
bool is_windows(void);

/**
 * @brief Check if the current OS is Unix-like (Linux, macOS, BSD, etc.).
 * @return true if running on a Unix-like system, false otherwise.
 */
bool is_unix(void);

/**
 * @brief Check if the current OS is Linux.
 * @return true if running on Linux, false otherwise.
 */
bool is_linux(void);

/**
 * @brief Check if the current OS is macOS.
 * @return true if running on macOS, false otherwise.
 */
bool is_macos(void);

/**
 * @brief Get the human-readable name of the current OS.
 * @return A string describing the OS (e.g., "Linux", "Windows", "macOS").
 *         The returned string is statically allocated and must not be freed.
 */
const char *os_name(void);

#ifdef __cplusplus
}
#endif

#endif /* OS_CHECKING_H */
