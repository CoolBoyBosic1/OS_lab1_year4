// executor.cpp — запуск команд рядка.
//
// Схема класична для UNIX: fork() створює копію процесу оболонки,
// у копії execv() заміщує образ програмою, а батьківський процес
// чекає завершення дитини через waitpid().
//
// Команди, розділені символом '&', запускаються справді паралельно:
// спочатку в циклі створюються ВСІ дочірні процеси, і лише потім
// оболонка чекає на кожен із них.
#include "shell.hpp"

#include <cstdio>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>

namespace wish {
namespace {

// RAII-обгортка над файловим дескриптором: закриває його у деструкторі,
// тож жодного шляху виконання без close() не лишається.
class Fd {
public:
    Fd() noexcept = default;
    explicit Fd(int fd) noexcept : fd_(fd) {}
    ~Fd() { reset(); }

    Fd(const Fd&)            = delete;
    Fd& operator=(const Fd&) = delete;   

    Fd(Fd&& other) noexcept : fd_(std::exchange(other.fd_, -1)) {}
    Fd& operator=(Fd&& other) noexcept {
        if (this != &other) {
            reset();
            fd_ = std::exchange(other.fd_, -1);
        }
        return *this;
    }

    [[nodiscard]] bool valid() const noexcept { return fd_ >= 0; }
    [[nodiscard]] int  get()   const noexcept { return fd_; }

    void reset() noexcept {
        if (fd_ >= 0) {
            ::close(fd_);
            fd_ = -1;
        }
    }

private:
    int fd_ = -1;
};

// Спрямовує stdout і stderr дочірнього процесу у файл.
// Файл створюється або обрізається до нуля (O_TRUNC).
bool redirect_output(const std::string& file) {
    const Fd fd{::open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644)};
    if (!fd.valid()) {
        report_error();
        return false;
    }
    // dup2 закриває дескриптор 1 (чи 2) і робить його копією fd —
    // після цього все, що програма пише «на екран», іде у файл.
    if (::dup2(fd.get(), STDOUT_FILENO) < 0 || ::dup2(fd.get(), STDERR_FILENO) < 0) {
        report_error();
        return false;
    }
    return true;   // оригінальний fd закриє деструктор Fd
}

}  // namespace

pid_t Shell::spawn(const Command& cmd, const std::string& program) const {
    // argv для execv() будуємо ДО fork(): після fork() у дитині безпечно
    // викликати лише async-signal-safe функції, а виділення пам'яті нею не є.
    // const_cast безпечний — execv() свій argv не змінює, такий у неї
    // історичний прототип.
    std::vector<char*> raw_argv;
    raw_argv.reserve(cmd.argv.size() + 1);
    for (const std::string& word : cmd.argv) {
        raw_argv.push_back(const_cast<char*>(word.c_str()));
    }
    raw_argv.push_back(nullptr);

    std::fflush(nullptr);   // щоб буфери не продублювались у дитині

    const pid_t pid = ::fork();
    if (pid < 0) {
        report_error();
        return -1;
    }

    if (pid == 0) {
        // дочірній процес
        if (cmd.outfile && !redirect_output(*cmd.outfile)) {
            ::_exit(1);
        }
        ::execv(program.c_str(), raw_argv.data());
        // сюди потрапляємо, лише якщо execv() зазнав невдачі
        report_error();
        ::_exit(1);   // саме _exit: не чіпаємо буфери, успадковані від батька
    }

    return pid;
}

bool Shell::execute_line(std::string_view line) {
    const CommandLine commands = parse_line(line);

    std::vector<pid_t> children;
    children.reserve(commands.size());
    bool shutdown = false;

    for (const Command& cmd : commands) {
        if (cmd.bad) {            // некоректне перенаправлення
            report_error();
            continue;
        }
        if (cmd.argv.empty()) {
            continue;
        }

        const Builtin result = run_builtin(cmd);
        if (result == Builtin::Exit) {
            shutdown = true;
            break;                // решту рядка не розбираємо
        }
        if (result == Builtin::Done) {
            continue;
        }

        const std::optional<std::string> program = path_.resolve(cmd.argv.front());
        if (!program) {
            report_error();       // команди немає в поточному шляху
            continue;
        }
        const pid_t pid = spawn(cmd, *program);
        if (pid > 0) {
            children.push_back(pid);
        }
    }

    // Чекаємо всіх дітей — навіть якщо виконано exit, сиріт не лишаємо.
    // Наступний рядок почнеться лише після завершення поточного.
    for (const pid_t pid : children) {
        ::waitpid(pid, nullptr, 0);
    }
    return shutdown;
}

}  // namespace wish
