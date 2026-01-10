#ifndef OS_CHECKING_H
#define OS_CHECKING_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool is_windows(void);
bool is_unix(void);
bool is_linux(void);
bool is_macos(void);
const char *os_name(void);

#ifdef __cplusplus
}
#endif

#endif /* OS_CHECKING_H */
