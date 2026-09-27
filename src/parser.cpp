// parser.cpp — перетворення текстового рядка на список команд.
#include "shell.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>

namespace wish {
namespace {

constexpr bool is_meta(char c) noexcept {
    return c == '>' || c == '&';
}

constexpr bool is_space(char c) noexcept {
    return c == ' ' || c == '\t' || c == '\n' ||
           c == '\v' || c == '\f' || c == '\r';
}

// Розбиття на лексеми: "ls>out" дає три лексеми — ls, >, out.
std::vector<std::string> tokenize(std::string_view line) {
    std::vector<std::string> tokens;

    for (std::size_t i = 0; i < line.size();) {
        if (is_space(line[i])) {
            ++i;
        } else if (is_meta(line[i])) {
            tokens.emplace_back(1, line[i]);
            ++i;
        } else {
            const std::size_t start = i;
            while (i < line.size() && !is_space(line[i]) && !is_meta(line[i])) {
                ++i;
            }
            tokens.emplace_back(line.substr(start, i - start));
        }
    }
    return tokens;
}

using Iterator = std::vector<std::string>::const_iterator;

// Будує команду з ділянки [begin, end) масиву лексем.
//
// Правила перенаправлення за специфікацією:
//   - '>' може траплятися щонайбільше один раз;
//   - ліворуч від нього має бути хоча б одне слово (сама команда);
//   - праворуч — рівно одне слово (ім'я файлу).
// Будь-яке порушення позначається прапорцем bad.
Command build_command(Iterator begin, Iterator end) {
    Command cmd;

    const Iterator arrow  = std::find(begin, end, ">");
    const auto redirects  = std::count(begin, end, ">");

    if (redirects == 0) {
        cmd.argv.assign(begin, end);
        return cmd;
    }
    if (redirects > 1 || arrow == begin || std::distance(arrow, end) != 2) {
        cmd.bad = true;
        return cmd;
    }

    cmd.argv.assign(begin, arrow);
    cmd.outfile = *(arrow + 1);
    return cmd;
}

}  // namespace

CommandLine parse_line(std::string_view line) {
    const std::vector<std::string> tokens = tokenize(line);
    CommandLine commands;

    // Ділимо потік лексем на ділянки, роздільник — '&'.
    Iterator segment = tokens.begin();
    while (segment != tokens.end()) {
        const Iterator boundary = std::find(segment, tokens.end(), "&");
        if (segment != boundary) {
            // Порожні ділянки ігноруємо: "&" чи "a &" — не помилка.
            commands.push_back(build_command(segment, boundary));
        }
        segment = (boundary == tokens.end()) ? boundary : boundary + 1;
    }
    return commands;
}

}  // namespace wish
