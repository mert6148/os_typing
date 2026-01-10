#include "terminal.h"
#include <stdio.h>
#include <string.h>

static int cmd_help(int argc, char **argv, void *ctx) {
    const struct term_cmd *cmds = (const struct term_cmd *)ctx;
    printf("Available commands:\n");
    for (int i = 0; cmds[i].name != NULL; ++i) {
        printf("  %s\t- %s\n", cmds[i].name, cmds[i].help ? cmds[i].help : "");
    }
    return 0;
}

static int cmd_echo(int argc, char **argv, void *ctx) {
    for (int i = 1; i < argc; ++i) {
        if (i != 1) putchar(' ');
        fputs(argv[i], stdout);
    }
    putchar('\n');
    return 0;
}

int main(void) {
    struct term_cmd cmds[] = {
        {"help", "Show this help", cmd_help, NULL},
        {"echo", "Print arguments", cmd_echo, NULL},
        {NULL, NULL, NULL, NULL}
    };

    /* make help see list */
    cmds[0].ctx = cmds;

    printf("Simple terminal demo. Type 'help' for commands, 'exit' to quit.\n");
    return terminal_run("os> ", cmds, 2);
}
