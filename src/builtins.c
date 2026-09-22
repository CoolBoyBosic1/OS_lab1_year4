/*
 * builtins.c — команди, які оболонка виконує сама, без fork()/exec().
 *
 * Вони мусять бути вбудованими: cd змінює робочий каталог самого
 * процесу оболонки, а дочірній процес змінити його батькові не може.
 */
#include "shell.h"

#include <string.h>
#include <unistd.h>

builtin_result builtin_run(command *cmd) {
    char **argv = cmd->argv.items;
    size_t argc = cmd->argv.count;

    if (strcmp(argv[0], "exit") == 0) {
        /* exit не приймає жодного аргументу */
        if (argc != 1) {
            wish_error();
            return BUILTIN_DONE;
        }
        return BUILTIN_EXIT;
    }

    if (strcmp(argv[0], "cd") == 0) {
        /* cd приймає рівно один аргумент */
        if (argc != 2 || chdir(argv[1]) != 0) {
            wish_error();
        }
        return BUILTIN_DONE;
    }

    return BUILTIN_NONE;
}
