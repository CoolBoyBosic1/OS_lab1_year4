/*
 * path.c — список каталогів, у яких оболонка шукає виконувані файли.
 *
 * Змінна середовища PATH тут свідомо НЕ використовується: за умовою
 * оболонка має власний шлях, який змінюється лише командою path.
 */
#include "shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static strvec search_path;

void path_init(void) {
    strvec_init(&search_path);
    strvec_push(&search_path, xstrdup("/bin"));
}

/*
 * path перезаписує список повністю (а не додає до нього).
 * Виклик "path" без аргументів робить список порожнім — після цього
 * жодну зовнішню програму запустити не вдасться, лише вбудовані команди.
 */
void path_set(char *const *dirs, size_t n) {
    strvec_free(&search_path);          /* strvec_free() лишає вектор порожнім */
    for (size_t i = 0; i < n; i++) {
        strvec_push(&search_path, xstrdup(dirs[i]));
    }
}

/*
 * Повертає перший шлях "каталог/програма", який існує й позначений
 * як виконуваний, або NULL. Рядок треба звільнити через free().
 */
char *path_resolve(const char *program) {
    for (size_t i = 0; i < search_path.count; i++) {
        const char *dir  = search_path.items[i];
        size_t      size = snprintf(NULL, 0, "%s/%s", dir, program) + 1;
        char       *full = xmalloc(size);

        snprintf(full, size, "%s/%s", dir, program);
        if (access(full, X_OK) == 0) {
            return full;
        }
        free(full);
    }
    return NULL;
}

void path_free(void) {
    strvec_free(&search_path);
}
