/*
 * parser.c — перетворення текстового рядка на структуру command.
 */
#include "shell.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* Символи, що є самостійними лексемами й не потребують пробілів навколо. */
static int is_meta(char c) {
    return c == '>' || c == '&';
}

/*
 * Розбиття на лексеми. Роздільник — будь-який пробільний символ
 * (зокрема '\n', який лишає після себе getline()); метасимволи
 * відокремлюються самі, тож "ls>out" дає три лексеми: ls, >, out.
 */
static void tokenize(const char *line, strvec *tokens) {
    const char *p = line;

    while (*p != '\0') {
        if (isspace((unsigned char)*p)) {
            p++;
            continue;
        }
        if (is_meta(*p)) {
            strvec_push(tokens, xstrndup(p, 1));
            p++;
            continue;
        }
        const char *start = p;
        while (*p != '\0' && !isspace((unsigned char)*p) && !is_meta(*p)) {
            p++;
        }
        strvec_push(tokens, xstrndup(start, (size_t)(p - start)));
    }
}

/*
 * Будує команду з готового масиву лексем.
 *
 * Правила перенаправлення за специфікацією:
 *   - '>' може траплятися щонайбільше один раз;
 *   - ліворуч від нього має бути хоча б одне слово (сама команда);
 *   - праворуч — рівно одне слово (ім'я файлу).
 * Будь-яке порушення позначається прапорцем bad.
 */
static void build_command(char **tokens, size_t n, command *out) {
    strvec_init(&out->argv);
    out->outfile = NULL;
    out->bad     = 0;

    size_t redirects = 0;
    size_t first     = n;

    for (size_t i = 0; i < n; i++) {
        if (strcmp(tokens[i], ">") == 0) {
            if (redirects == 0) {
                first = i;
            }
            redirects++;
        }
    }

    if (redirects == 0) {
        for (size_t i = 0; i < n; i++) {
            strvec_push(&out->argv, xstrdup(tokens[i]));
        }
        return;
    }

    if (redirects > 1 || first == 0 || n - first != 2) {
        out->bad = 1;
        return;
    }

    for (size_t i = 0; i < first; i++) {
        strvec_push(&out->argv, xstrdup(tokens[i]));
    }
    out->outfile = xstrdup(tokens[first + 1]);
}

void cmdlist_init(command_list *list) {
    list->items    = NULL;
    list->count    = 0;
    list->capacity = 0;
}

static command *cmdlist_add(command_list *list) {
    if (list->count == list->capacity) {
        list->capacity = (list->capacity == 0) ? 4 : list->capacity * 2;
        list->items    = xrealloc(list->items, list->capacity * sizeof(command));
    }
    return &list->items[list->count++];
}

void cmdlist_free(command_list *list) {
    for (size_t i = 0; i < list->count; i++) {
        strvec_free(&list->items[i].argv);
        free(list->items[i].outfile);
    }
    free(list->items);
    cmdlist_init(list);
}

void parse_line(const char *line, command_list *out) {
    strvec tokens;
    strvec_init(&tokens);
    tokenize(line, &tokens);

    /*
     * Ділимо потік лексем на ділянки, роздільник — '&'.
     * Індекс i == tokens.count обробляє останню ділянку.
     */
    size_t start = 0;
    for (size_t i = 0; i <= tokens.count; i++) {
        int boundary = (i == tokens.count) || (strcmp(tokens.items[i], "&") == 0);
        if (!boundary) {
            continue;
        }
        size_t n = i - start;
        if (n > 0) {   /* порожні ділянки ігноруємо: "&" чи "a &" — не помилка */
            build_command(&tokens.items[start], n, cmdlist_add(out));
        }
        start = i + 1;
    }

    strvec_free(&tokens);
}
