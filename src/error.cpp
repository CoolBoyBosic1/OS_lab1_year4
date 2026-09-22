// error.cpp — єдине повідомлення про помилку, дозволене специфікацією.
#include "shell.hpp"

#include <cstring>
#include <unistd.h>

namespace wish {

void report_error() noexcept {
    // Текст і розмір буфера — дослівно з умови задачі.
    char error_message[30] = "An error has occurred\n";

    // write() — системний виклик без буферизації, тому повідомлення
    // гарантовано опиняється в правильному порядку відносно виводу
    // дочірніх процесів. Результат зберігаємо, бо write() позначено
    // атрибутом warn_unused_result; реагувати на нього немає сенсу:
    // якщо не вдалося написати навіть у stderr, повідомляти вже нікуди.
    const ssize_t written =
        ::write(STDERR_FILENO, error_message, std::strlen(error_message));
    static_cast<void>(written);
}

}  // namespace wish
