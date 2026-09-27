#! /bin/bash
#
# Наочний прогін офіційних тестів: для кожного тесту показує опис, команду,
# вхідний файл, очікуваний і фактичний stdout / stderr / код повернення.
# Вердикт passed/failed бере з офіційного тестера (tester/run-tests.sh).
#
#   ./show-tests.sh          — усі 22 тести
#   ./show-tests.sh 7 22     — лише тести 7 і 22
#   ./show-tests.sh -p       — пауза (Enter) після кожного тесту, зручно для скріншотів
#
cd "$(dirname "$0")" || exit 1

if ! [[ -x wish ]]; then
    echo "виконуваний файл ./wish не знайдено — спочатку зберіть проєкт (make)"
    exit 1
fi

B=$'\033[1m'; DIM=$'\033[2m'; G=$'\033[32m'; R=$'\033[31m'
C=$'\033[36m'; Y=$'\033[33m'; N=$'\033[0m'

pause=0
nums=()
for arg in "$@"; do
    if [[ $arg == -p ]]; then pause=1; else nums+=("$arg"); fi
done
if (( ${#nums[@]} == 0 )); then
    for f in tests/*.run; do nums+=("$(basename "$f" .run)"); done
    mapfile -t nums < <(printf '%s\n' "${nums[@]}" | sort -n)
fi
total=$(ls tests/*.run | wc -l)

# Друкує файл з рамкою ліворуч; табуляції показує як ⇥, пробіли в кінці як ·
show_file() {
    local file=$1 color=$2
    if [[ ! -s $file ]]; then
        echo "    ${DIM}│ (порожньо)${N}"
        return
    fi
    perl -pe 's/\t/⇥/g; s/( +)$/"·" x length($1)/e' "$file" |
        while IFS= read -r line; do echo "    ${DIM}│${N} ${color}${line}${N}"; done
}

# Порівнює очікуваний і фактичний файл одного потоку
compare() {
    local name=$1 exp=$2 got=$3 color=$4
    if diff -q "$exp" "$got" >/dev/null; then
        echo "  ${B}${name}${N}  ${G}✔ збігається з еталоном${N}"
        show_file "$got" "$color"
    else
        echo "  ${B}${name}${N}  ${R}✘ НЕ збігається${N}"
        echo "    ${DIM}очікується ($exp):${N}"; show_file "$exp" "$color"
        echo "    ${DIM}отримано ($got):${N}";   show_file "$got" "$color"
    fi
}

passed=0; failed=()
for n in "${nums[@]}"; do
    if [[ ! -f tests/$n.run ]]; then
        echo "${R}тесту $n не існує${N}"; continue
    fi

    verdict=$(./tester/run-tests.sh -s -t "$n" | sed 's/\x1b\[[0-9;]*m//g')

    echo "${C}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${N}"
    echo "${B}${C}ТЕСТ $n / $total${N}  ${B}$(cat "tests/$n.desc")${N}"
    echo "${C}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${N}"
    [[ -f tests/$n.pre ]] && echo "  ${B}підготовка${N}  ${Y}$(cat "tests/$n.pre")${N}"
    echo "  ${B}команда${N}     ${Y}$(cat "tests/$n.run")${N}"
    if [[ -f tests/$n.in ]]; then
        echo "  ${B}вхід${N} ${DIM}(tests/$n.in — команди для wish)${N}"
        show_file "tests/$n.in" ""
    fi
    compare "stdout" "tests/$n.out" "tests-out/$n.out" ""
    compare "stderr" "tests/$n.err" "tests-out/$n.err" "$R"
    exp_rc=$(cat "tests/$n.rc"); got_rc=$(cat "tests-out/$n.rc")
    if [[ $exp_rc == "$got_rc" ]]; then
        echo "  ${B}код повернення${N}  ${got_rc}  ${G}✔ очікувався ${exp_rc}${N}"
    else
        echo "  ${B}код повернення${N}  ${got_rc}  ${R}✘ очікувався ${exp_rc}${N}"
    fi

    if [[ $verdict == *passed* ]]; then
        echo "  ${B}${G}▶ ТЕСТ $n: PASSED${N}  ${DIM}(офіційний тестер)${N}"
        ((passed++))
    else
        echo "  ${B}${R}▶ ТЕСТ $n: FAILED${N}  ${DIM}(офіційний тестер)${N}"
        failed+=("$n")
    fi
    echo

    if (( pause )); then read -r -p "${DIM}Enter — наступний тест${N} " _ </dev/tty; echo; fi
done

echo "${C}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${N}"
if (( ${#failed[@]} == 0 )); then
    echo "${B}${G}ПІДСУМОК: пройдено ${passed} з ${#nums[@]}${N}"
else
    echo "${B}${R}ПІДСУМОК: пройдено ${passed} з ${#nums[@]}, провалено: ${failed[*]}${N}"
    exit 1
fi
