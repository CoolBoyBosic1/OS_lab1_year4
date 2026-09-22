/*
 * shell.h — спільні оголошення оболонки wish.
 *
 * _POSIX_C_SOURCE визначено тут (до будь-яких #include), щоб getline(),
 * fork(), execv() тощо були видимі навіть під час компіляції в режимі
 * строгого -std=c11.
 */
#ifndef WISH_SHELL_H
#define WISH_SHELL_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stddef.h>

/* ------------------------------------------------------------------ */
/* Повідомлення про помилку                                            */
/* ------------------------------------------------------------------ */

/*
 * Специфікація OSTEP дозволяє рівно одне повідомлення про помилку,
 * записане саме у stderr саме цими байтами.
 */
void wish_error(void);

/* ------------------------------------------------------------------ */
/* Допоміжні обгортки над malloc                                       */
/* ------------------------------------------------------------------ */
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
char *xstrdup(const char *s);
char *xstrndup(const char *s, size_t n);

/* ------------------------------------------------------------------ */
/* Динамічний масив рядків                                             */
/* ------------------------------------------------------------------ */

/*
 * Вектор завжди тримає items[count] == NULL, тому поле items можна
 * без копіювання передавати в execv() як argv.
 */
typedef struct {
    char  **items;
    size_t  count;
    size_t  capacity;
} strvec;

void strvec_init(strvec *v);
void strvec_push(strvec *v, char *owned);  /* вектор забирає володіння рядком */
void strvec_free(strvec *v);

#endif /* WISH_SHELL_H */
