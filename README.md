# BEJ Parser in C

Утиліта на чистому C для декодування бінарного BEJ в JSON.  
Збирається CMake + GCC/Clang.

**Ключові можливості**
- Вхід: **словник** (CSV) + **BEJ-бінар** → вихід: **JSON (UTF-8)**.
- Базові типи: `int`, `string`, `set` (+ масиви в демо: `array-int`).
- CMake-проект, unit-тести на ULEB128/varint, є Doxygen-конфіг.

> В рамках спрощення завдання: анотації не розбираються, підтримується мінімальний набір типів. Для демонстрації архітектури цього достатньо.


## Збірка

```bash
git clone https://github.com/AlinaShcherbak1/BEJ_Parser.git
cd BEJ_Parser
cmake -S . -B build-ninja -G Ninja
cmake --build build-ninja
```

## Запуск тестів

```
ctest --test-dir build-ninja -V
```
Очікуваний результат:

. ULEB128 basic<br>
. ULEB128 multibyte<br>
OK 4 tests

## Декодування прикладу

У репозиторії є файли examples/:
- toy_dict.csv — словник
- example_bej.bin — бінарний файл BEJ
- example.json — еталонний JSON

Запуск Windows PowerShell:
```
& .\build-ninja\bej2json.exe `
  -d ".\examples\toy_dict.csv" `
  -b ".\examples\example_bej.bin" `
  -o ".\out.json" `
  --skip-anno -v
```

bash
```
./build-ninja/bej2json \
  -d ./examples/toy_dict.csv \
  -b ./examples/example_bej.bin \
  -o ./out.json \
  --skip-anno -v
  ```
