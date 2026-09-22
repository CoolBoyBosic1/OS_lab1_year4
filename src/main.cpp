// wish — Wisconsin Shell (проєкт processes-shell з курсу OSTEP).
//
// Два режими роботи:
//   ./wish            — інтерактивний: запрошення "wish> " і читання stdin;
//   ./wish файл       — пакетний (batch): команди читаються з файлу,
//                       запрошення не друкується;
//   ./wish a b ...    — помилка, вихід із кодом 1.
#include "shell.hpp"

#include <fstream>
#include <iostream>
#include <istream>
#include <string>

namespace {

// Цикл «прочитати рядок — виконати».
void repl(std::istream& input, bool interactive) {
    std::string line;

    while (true) {
        if (interactive) {
            std::cout << "wish> " << std::flush;   // запрошення одразу на екран
        }
        if (!std::getline(input, line)) {
            break;          // EOF (Ctrl-D або кінець файлу)
        }
        if (wish::execute_line(line)) {
            break;          // виконано команду exit
        }
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc > 2) {                     // більше одного файлу — помилка
        wish::report_error();
        return 1;
    }

    if (argc == 2) {
        std::ifstream batch(argv[1]);
        if (!batch) {                   // пакетний файл не відкривається
            wish::report_error();
            return 1;
        }
        repl(batch, false);
        return 0;
    }

    repl(std::cin, true);
    return 0;
}
