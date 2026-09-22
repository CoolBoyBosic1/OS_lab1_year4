/*
 * executor.c — запуск зовнішніх програм.
 *
 * Схема класична для UNIX: fork() створює копію процесу оболонки,
 * у копії execv() заміщує образ програмою, а батьківський процес
 * чекає завершення дитини через waitpid().
 */
#include "shell.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * Спрямовує stdout і stderr дочірнього процесу у файл.
 * Файл створюється або обрізається до нуля (O_TRUNC).
 * Повертає 1 при успіху.
 */
static int redirect_output(const char *file) {
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        wish_error();
        return 0;
    }
    if (dup2(fd, STDOUT_FILENO) < 0 || dup2(fd, STDERR_FILENO) < 0) {
        wish_error();
        close(fd);
        return 0;
    }
    close(fd);
    return 1;
}

/* Породжує процес для однієї команди. Повертає pid або -1. */
static pid_t spawn(command *cmd, const char *program) {
    fflush(NULL);          /* щоб буфери stdio не продублювались у дитині */

    pid_t pid = fork();
    if (pid < 0) {
        wish_error();
        return -1;
    }

    if (pid == 0) {
        /* дочірній процес */
        if (cmd->outfile != NULL && !redirect_output(cmd->outfile)) {
            _exit(1);
        }
        execv(program, cmd->argv.items);
        /* сюди потрапляємо, лише якщо execv() зазнав невдачі */
        wish_error();
        _exit(1);
    }

    return pid;
}

int execute_line(const char *line) {
    command cmd;
    int     shutdown = 0;

    if (!parse_line(line, &cmd)) {
        command_free(&cmd);
        return 0;                      /* порожній рядок — не помилка */
    }

    if (cmd.bad) {                     /* некоректне перенаправлення */
        wish_error();
        command_free(&cmd);
        return 0;
    }

    switch (builtin_run(&cmd)) {
        case BUILTIN_EXIT:
            shutdown = 1;
            break;
        case BUILTIN_DONE:
            break;
        case BUILTIN_NONE: {
            char *program = path_resolve(cmd.argv.items[0]);
            if (program == NULL) {
                wish_error();          /* команди немає в поточному шляху */
                break;
            }
            pid_t pid = spawn(&cmd, program);
            free(program);
            if (pid > 0) {
                waitpid(pid, NULL, 0);
            }
            break;
        }
    }

    command_free(&cmd);
    return shutdown;
}
