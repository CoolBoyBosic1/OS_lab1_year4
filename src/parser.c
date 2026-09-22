/*
 * parser.c — перетворення текстового рядка на структуру command.
 */
#include "shell.h"

#include <ctype.h>
#include <stdlib.h>

/*
 * Розбиття на слова. Роздільник — будь-який пробільний символ,
 * зокрема символ переводу рядка, який лишає після себе getline().
 */
static void tokenize(const char *line, strvec *tokens) {
    const char *p = line;

    while (*p != '\0') {
        if (isspace((unsigned char)*p)) {
            p++;
            continue;
        }
        const char *start = p;
        while (*p != '\0' && !isspace((unsigned char)*p)) {
            p++;
        }
        strvec_push(tokens, xstrndup(start, (size_t)(p - start)));
    }
}

int parse_line(const char *line, command *out) {
    strvec_init(&out->argv);
    tokenize(line, &out->argv);
    return out->argv.count > 0;
}

void command_free(command *cmd) {
    strvec_free(&cmd->argv);
}
