/*
 * util.c — повідомлення про помилку, обгортки над malloc і вектор рядків.
 */
#include "shell.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void wish_error(void) {
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

/*
 * Брак пам'яті для оболонки — невиправна ситуація: далі працювати
 * немає як, тож просто повідомляємо про помилку й виходимо.
 */
void *xmalloc(size_t size) {
    void *p = malloc(size);
    if (p == NULL) {
        wish_error();
        exit(1);
    }
    return p;
}

void *xrealloc(void *ptr, size_t size) {
    void *p = realloc(ptr, size);
    if (p == NULL) {
        wish_error();
        exit(1);
    }
    return p;
}

char *xstrdup(const char *s) {
    return xstrndup(s, strlen(s));
}

char *xstrndup(const char *s, size_t n) {
    char *copy = xmalloc(n + 1);
    memcpy(copy, s, n);
    copy[n] = '\0';
    return copy;
}

void strvec_init(strvec *v) {
    v->items    = NULL;
    v->count    = 0;
    v->capacity = 0;
}

/* Забезпечує місце щонайменше під need вказівників. */
static void strvec_reserve(strvec *v, size_t need) {
    if (need <= v->capacity) {
        return;
    }
    size_t cap = (v->capacity == 0) ? 8 : v->capacity * 2;
    while (cap < need) {
        cap *= 2;
    }
    v->items    = xrealloc(v->items, cap * sizeof(char *));
    v->capacity = cap;
}

void strvec_push(strvec *v, char *owned) {
    strvec_reserve(v, v->count + 2);   /* +1 елемент і +1 завершальний NULL */
    v->items[v->count++] = owned;
    v->items[v->count]   = NULL;
}

void strvec_free(strvec *v) {
    for (size_t i = 0; i < v->count; i++) {
        free(v->items[i]);
    }
    free(v->items);
    strvec_init(v);
}
