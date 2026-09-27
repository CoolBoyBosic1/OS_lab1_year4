// error.cpp — єдине повідомлення про помилку, дозволене специфікацією.
#include "shell.hpp"

#include <cstring>
#include <unistd.h>

namespace wish {

void report_error() noexcept {
    // Текст і розмір буфера — дослівно з умови задачі.
    char error_message[30] = "An error has occurred\n";

    const ssize_t written =
        ::write(STDERR_FILENO, error_message, std::strlen(error_message));
    static_cast<void>(written);
}

}  // namespace wish
