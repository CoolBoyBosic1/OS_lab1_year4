#include "shell.hpp"

#include <fstream>
#include <iostream>
#include <istream>
#include <string>

namespace {

void repl(std::istream& input, bool interactive) {
    wish::Shell shell;      // весь стан оболонки живе тут
    std::string line;

    while (true) {
        if (interactive) {
            std::cout << "wish> " << std::flush;   // запрошення одразу на екран
        }
        if (!std::getline(input, line)) {
            break;
        }
        if (shell.execute_line(line)) {
            break;
        }
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc > 2) {
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
