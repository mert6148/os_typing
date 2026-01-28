#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * @file terminal.c
 * @brief Implementation of a simple command-line terminal/REPL loop.
 * 
 * Provides a portable line-based interface for applications to expose
 * pluggable commands. Handles input parsing, tokenization, and dispatch
 * to registered command handlers.
 */

/**
 * @brief Portable getline() fallback for MSVC and older platforms.
 * @param lineptr Pointer to line buffer.
 * @param n Pointer to buffer capacity.
 * @param stream Input stream (typically stdin).
 * @return Number of characters read, or -1 on EOF/error.
 */
static ssize_t portable_getline(char **lineptr, size_t *n, FILE *stream) {
#ifdef _MSC_VER
    /* simple fallback using fgets */
    char buf[1024];
    if (!fgets(buf, sizeof(buf), stream)) return -1;
    size_t len = strlen(buf);
    *lineptr = (char *)realloc(*lineptr, len + 1);
    if (!*lineptr) return -1;
    memcpy(*lineptr, buf, len + 1);
    *n = len + 1;
    return (ssize_t)len;
#else
    return getline(lineptr, n, stream);
#endif
}

static void trim_newline(char *s) {
    size_t i = strlen(s);
    while (i > 0 && (s[i-1] == '\n' || s[i-1] == '\r')) s[--i] = '\0';
}

int terminal_find_cmd(const struct term_cmd *cmds, int ncmds, const char *name) {
    for (int i = 0; i < ncmds; ++i) {
        if (cmds[i].name && strcmp(cmds[i].name, name) == 0) return i;
    }
    return -1;
}

int terminal_run(const char *prompt, const struct term_cmd *cmds, int ncmds) {
    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    int ret = 0;

    while (1) {
        if (prompt) fputs(prompt, stdout);
        fflush(stdout);

        linelen = portable_getline(&line, &linecap, stdin);
        if (linelen < 0) { /* EOF or error */
            putchar('\n');
            break;
        }

        trim_newline(line);

        /* skip empty lines */
        char *p = line;
        while (*p && isspace((unsigned char)*p)) ++p;
        if (*p == '\0') continue;

        /* tokenize (simple whitespace split) */
        char *argv[TERM_MAX_ARGS];
        int argc = 0;
        char *saveptr = NULL;
        char *tok = strtok_r(line, " \t", &saveptr);
        while (tok && argc < TERM_MAX_ARGS) {
            argv[argc++] = tok;
            tok = strtok_r(NULL, " \t", &saveptr);
        }
        if (argc == 0) continue;

        /* builtin `exit` */
        if (strcmp(argv[0], "exit") == 0 || strcmp(argv[0], "quit") == 0) {
            break;
        }

        int idx = terminal_find_cmd(cmds, ncmds, argv[0]);
        if (idx >= 0) {
            int r = cmds[idx].fn(argc, argv, cmds[idx].ctx);
            if (r != 0) ret = r;
        } else {
            fprintf(stderr, "Unknown command: %s\n", argv[0]);
        }
    }

    free(line);
    return ret;
}
