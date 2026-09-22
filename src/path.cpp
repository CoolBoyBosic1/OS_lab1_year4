// path.cpp — список каталогів, у яких оболонка шукає виконувані файли.
//
// Змінна середовища PATH тут свідомо НЕ використовується: за умовою
// оболонка має власний шлях, який змінюється лише командою path.
#include "shell.hpp"

#include <unistd.h>

namespace wish::path {
namespace {

// Функціональний статик: ініціалізується під час першого звернення,
// тож порядок статичної ініціалізації між файлами ролі не грає.
std::vector<std::string>& dirs() {
    static std::vector<std::string> value{"/bin"};
    return value;
}

}  // namespace

void set(std::vector<std::string> new_dirs) {
    dirs() = std::move(new_dirs);
}

std::optional<std::string> resolve(std::string_view program) {
    for (const std::string& dir : dirs()) {
        std::string full = dir;
        full += '/';
        full += program;

        if (::access(full.c_str(), X_OK) == 0) {
            return full;
        }
    }
    return std::nullopt;
}

}  // namespace wish::path
