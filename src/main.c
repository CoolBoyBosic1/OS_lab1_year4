/*
 * wish — Wisconsin Shell.
 * Крок 5: додано вбудовану команду path і пошук програм за шляхом.
 */
#include "shell.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char   *line    = NULL;
    size_t  cap     = 0;
    int     running = 1;

    path_init();

    while (running) {
        printf("wish> ");
        fflush(stdout);

        if (getline(&line, &cap, stdin) < 0) {
            break;
        }

        command cmd;
        if (parse_line(line, &cmd)) {
            if (builtin_run(&cmd) == BUILTIN_EXIT) {
                running = 0;
            }
        }
        command_free(&cmd);
    }

    free(line);
    path_free();
    return 0;
}
