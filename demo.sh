#! /bin/bash
#
# Демонстрація можливостей wish: кожен сценарій — окремий пакетний файл,
# виводиться його вміст, stdout/stderr оболонки та код повернення.
#   ./demo.sh
#
cd "$(dirname "$0")" || exit 1
export LC_ALL=C   # однакові (англійські) повідомлення системних програм

if ! [[ -x wish ]]; then
    echo "виконуваний файл ./wish не знайдено — спочатку зберіть проєкт (make)"
    exit 1
fi

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

n=0
# demo "назва" "рядки скрипта"...
demo() {
    local title=$1; shift
    n=$((n + 1))
    printf '%s\n' "$@" > "$tmp/script.txt"
    echo "=== $n. $title"
    echo "--- скрипт:"
    sed 's/^/    /' "$tmp/script.txt"
    echo "--- вивід:"
    (cd "$tmp" && "$OLDPWD/wish" script.txt) 2>&1 | sed 's/^/    /'
    echo "--- код повернення: ${PIPESTATUS[0]}"
    echo
}

demo "Зовнішні програми (fork + execv + waitpid)" \
    "echo hello from wish" \
    "uname -s"

demo "Вбудована cd" \
    "cd /usr" \
    "pwd" \
    "cd /tmp" \
    "pwd"

demo "Помилка програми — це її власний stderr, а не помилка оболонки" \
    "ls /nonexistent"

demo "Помилка cd: немає аргументу / забагато аргументів" \
    "cd" \
    "cd /a /b" \
    "echo shell continues"

demo "Вбудована path: програма шукається лише в заданих каталогах" \
    "path /nonexistent" \
    "whoami" \
    "path /tmp /usr/bin" \
    "whoami" \
    "path" \
    "echo path is empty now, so this fails"

demo "Перенаправлення виводу (stdout і stderr у файл)" \
    "echo written to file > out.txt" \
    "ls /nonexistent>err.txt" \
    "cat out.txt" \
    "cat err.txt"

demo "Некоректне перенаправлення" \
    "ls >" \
    "ls > a b" \
    "ls > a > b" \
    "> a"

demo "Паралельні команди через & (три sleep 2 — разом 2 с, а не 6)" \
    "date +%T" \
    "sleep 2 & sleep 2 & sleep 2" \
    "date +%T" \
    "echo A & echo B & echo C"

demo "Довільні пробіли, табуляції та порожні рядки" \
    "     echo    spaced     out   " \
    "" \
    "	" \
    "&" \
    "echo still alive"

demo "exit з аргументом — помилка; без аргументу — вихід" \
    "exit 1" \
    "echo not exited yet" \
    "exit" \
    "echo never printed"

echo "=== $((n + 1)). Неправильний виклик оболонки"
echo "--- ./wish a b"
./wish a b 2>&1 | sed 's/^/    /'; echo "--- код повернення: ${PIPESTATUS[0]}"
echo "--- ./wish no-such-file"
./wish no-such-file 2>&1 | sed 's/^/    /'; echo "--- код повернення: ${PIPESTATUS[0]}"
