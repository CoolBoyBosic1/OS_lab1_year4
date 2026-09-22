/*
 * executor.c — запуск команд рядка.
 *
 * Схема класична для UNIX: fork() створює копію процесу оболонки,
 * у копії execv() заміщує образ програмою, а батьківський процес
 * чекає завершення дитини через waitpid().
 *
 * Команди, розділені символом '&', запускаються справді паралельно:
 * спочатку в циклі створюються ВСІ дочірні процеси, і лише потім
 * оболонка чекає на кожен із них.
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
    command_list cmds;
    int          shutdown = 0;

    cmdlist_init(&cmds);
    parse_line(line, &cmds);

    if (cmds.count == 0) {
        cmdlist_free(&cmds);
        return 0;                      /* порожній рядок — не помилка */
    }

    pid_t *pids  = xmalloc(cmds.count * sizeof(pid_t));
    size_t npids = 0;

    for (size_t i = 0; i < cmds.count && !shutdown; i++) {
        command *cmd = &cmds.items[i];

        if (cmd->bad) {                /* некоректне перенаправлення */
            wish_error();
            continue;
        }

        switch (builtin_run(cmd)) {
            case BUILTIN_EXIT:
                shutdown = 1;
                break;
            case BUILTIN_DONE:
                break;
            case BUILTIN_NONE: {
                char *program = path_resolve(cmd->argv.items[0]);
                if (program == NULL) {
                    wish_error();      /* команди немає в поточному шляху */
                    break;
                }
                pid_t pid = spawn(cmd, program);
                free(program);
                if (pid > 0) {
                    pids[npids++] = pid;
                }
                break;
            }
        }
    }

    /* Чекаємо всіх дітей: наступний рядок має виконуватись після них. */
    for (size_t i = 0; i < npids; i++) {
        waitpid(pids[i], NULL, 0);
    }

    free(pids);
    cmdlist_free(&cmds);
    return shutdown;
}
