#! /bin/bash
#
# Запуск офіційних тестів OSTEP.
#   ./test-wish.sh       — зупинитись на першому невдалому тесті
#   ./test-wish.sh -c    — пройти всі тести, не зупиняючись
#   ./test-wish.sh -v    — докладний вивід (опис кожного тесту)
#   ./test-wish.sh -t 11 — лише тест номер 11
#
cd "$(dirname "$0")" || exit 1

if ! [[ -x wish ]]; then
    echo "виконуваний файл ./wish не знайдено — спочатку зберіть проєкт (make)"
    exit 1
fi

./tester/run-tests.sh "$@"
