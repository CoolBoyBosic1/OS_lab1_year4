// path.cpp — пошук виконуваного файлу за власним шляхом оболонки.
#include "shell.hpp"

#include <unistd.h>

namespace wish {

std::optional<std::string> SearchPath::resolve(std::string_view program) const {
    for (const std::string& dir : dirs_) {
        std::string full = dir;
        full += '/';
        full += program;

        if (::access(full.c_str(), X_OK) == 0) {
            return full;
        }
    }
    return std::nullopt;
}

}  // namespace wish
