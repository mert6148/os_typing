#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TERM_MAX_ARGS 16

typedef int (*term_cmd_fn)(int argc, char **argv, void *ctx);

struct term_cmd {
    const char *name;
    const char *help;
    term_cmd_fn fn;
    void *ctx;
};

/* Run a simple line-based terminal loop. Returns when `exit` command is executed
   or on EOF. `prompt` must be a NUL-terminated string. `cmds` is an array of
   `ncmds` commands. */
int terminal_run(const char *prompt, const struct term_cmd *cmds, int ncmds);

/* Helper to register builtin commands with a mutable array. */
int terminal_find_cmd(const struct term_cmd *cmds, int ncmds, const char *name);

#ifdef __cplusplus
}
#endif

#endif /* TERMINAL_H */
