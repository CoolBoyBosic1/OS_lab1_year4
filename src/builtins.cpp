// builtins.cpp — команди, які оболонка виконує сама, без fork()/exec().
#include "shell.hpp"

#include <unistd.h>

namespace wish {

Shell::Builtin Shell::run_builtin(const Command& cmd) {
    const std::string& name = cmd.argv.front();

    if (name == "exit") {
        // exit не приймає жодного аргументу
        if (cmd.argv.size() != 1) {
            report_error();
            return Builtin::Done;
        }
        return Builtin::Exit;
    }

    if (name == "cd") {
        // cd приймає рівно один аргумент
        if (cmd.argv.size() != 2 || ::chdir(cmd.argv[1].c_str()) != 0) {
            report_error();
        }
        return Builtin::Done;
    }

    if (name == "path") {
        // усі аргументи, крім самого слова path, стають новим шляхом
        path_.set({cmd.argv.begin() + 1, cmd.argv.end()});
        return Builtin::Done;
    }

    return Builtin::None;
}

}  // namespace wish
