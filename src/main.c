/*
 * wish — Wisconsin Shell.
 * Крок 1: каркас циклу «прочитати рядок — виконати — повторити» (REPL).
 */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char   *line = NULL;   /* буфер, який getline() виділяє сам */
    size_t  cap  = 0;      /* поточний розмір цього буфера      */

    while (1) {
        printf("wish> ");
        fflush(stdout);

        if (getline(&line, &cap, stdin) < 0) {
            break;         /* EOF (Ctrl-D) — завершуємо роботу */
        }

        /* поки що нічого не робимо з прочитаним рядком */
    }

    free(line);
    return 0;
}
