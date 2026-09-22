/*
 * wish — Wisconsin Shell.
 * Крок 3: рядок розбирається на слова (майбутній argv для execv()).
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

        command cmd;
        if (parse_line(line, &cmd)) {
            /* порожні рядки просто пропускаємо, помилкою вони не є */
        }
        command_free(&cmd);
    }

    free(line);
    return 0;
}
