/*
 * wish — Wisconsin Shell.
 * Крок 2: цикл REPL плюс базова інфраструктура (помилки, вектор рядків).
 */
#include "shell.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char   *line = NULL;
    size_t  cap  = 0;

    while (1) {
        printf("wish> ");
        fflush(stdout);

        if (getline(&line, &cap, stdin) < 0) {
            break;
        }
    }

    free(line);
    return 0;
}
