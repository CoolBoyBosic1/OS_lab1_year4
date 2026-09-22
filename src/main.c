/*
 * wish — Wisconsin Shell.
 * Крок 6: оболонка вміє запускати зовнішні програми.
 */
#include "shell.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char   *line = NULL;
    size_t  cap  = 0;

    path_init();

    while (1) {
        printf("wish> ");
        fflush(stdout);

        if (getline(&line, &cap, stdin) < 0) {
            break;                     /* EOF — так само завершуємо роботу */
        }
        if (execute_line(line)) {
            break;                     /* виконано команду exit */
        }
    }

    free(line);
    path_free();
    return 0;
}
