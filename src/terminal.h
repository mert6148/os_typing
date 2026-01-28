#ifndef TERMINAL_H
#define TERMINAL_H

/**
 * @file terminal.h
 * @brief Simple line-based terminal/REPL loop for C programs.
 * 
 * Provides a pluggable command registration and execution framework,
 * allowing applications to expose a command-line interface with custom
 * handlers and help text.
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Maximum number of arguments per command. */
#define TERM_MAX_ARGS 16

/**
 * @brief Function pointer type for terminal command handlers.
 * @param argc Number of arguments (including command name as argv[0]).
 * @param argv Argument array (null-terminated strings).
 * @param ctx User-provided context pointer.
 * @return Exit code; 0 for success, non-zero for error.
 */
typedef int (*term_cmd_fn)(int argc, char **argv, void *ctx);

/**
 * @brief Terminal command definition.
 * 
 * Describes a single command available in the terminal loop.
 */
struct term_cmd {
    const char *name;       /**< Command name (e.g., "help", "echo"). */
    const char *help;       /**< Help text displayed by the help command. */
    term_cmd_fn fn;         /**< Function pointer to execute the command. */
    void *ctx;              /**< User-provided context passed to fn(). */
};

/**
 * @brief Run a simple line-based terminal loop.
 * 
 * Prompts the user for input, parses commands, and dispatches to registered
 * handlers. Continues until "exit" command or EOF.
 * 
 * @param prompt NUL-terminated prompt string (e.g., "> ").
 * @param cmds Array of available commands.
 * @param ncmds Number of commands in the array.
 * @return Exit code from the final command or terminal loop status.
 */
int terminal_run(const char *prompt, const struct term_cmd *cmds, int ncmds);

/**
 * @brief Find a command by name in the command array.
 * 
 * @param cmds Array of commands.
 * @param ncmds Number of commands.
 * @param name Command name to find.
 * @return Index of the command if found; -1 if not found.
 */
int terminal_find_cmd(const struct term_cmd *cmds, int ncmds, const char *name);

#ifdef __cplusplus
}
#endif

#endif /* TERMINAL_H */
