/*
 * wish — Wisconsin Shell (проєкт processes-shell з курсу OSTEP).
 *
 * Два режими роботи:
 *   ./wish            — інтерактивний: запрошення "wish> " і читання stdin;
 *   ./wish файл       — пакетний (batch): команди читаються з файлу,
 *                       запрошення не друкується.
 *   ./wish a b ...    — помилка, вихід із кодом 1.
 */
#include "shell.h"

#include <stdio.h>
#include <stdlib.h>

/* Цикл «прочитати рядок — виконати». */
static void repl(FILE *input, int interactive) {
    char   *line = NULL;   /* буфер, який getline() виділяє та збільшує сам */
    size_t  cap  = 0;

    while (1) {
        if (interactive) {
            printf("wish> ");
            fflush(stdout);            /* запрошення має з'явитись одразу */
        }
        if (getline(&line, &cap, input) < 0) {
            break;                     /* EOF (Ctrl-D або кінець файлу) */
        }
        if (execute_line(line)) {
            break;                     /* виконано команду exit */
        }
    }

    free(line);
}

int main(int argc, char *argv[]) {
    FILE *input       = stdin;
    int   interactive = 1;

    if (argc > 2) {                    /* більше одного файлу — помилка */
        wish_error();
        return 1;
    }
    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (input == NULL) {           /* пакетний файл не відкривається */
            wish_error();
            return 1;
        }
        interactive = 0;
    }

    path_init();
    repl(input, interactive);
    path_free();

    if (input != stdin) {
        fclose(input);
    }
    return 0;
}
