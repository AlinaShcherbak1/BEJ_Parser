# BEJ Parser in C

Утиліта на чистому C для декодування бінарного BEJ в JSON.  
Підтримує словники DMTF DSP8010 (`*.bin`), збирається через CMake + GCC/Clang.

**Ключові можливості**
- **Вхід:** **major dictionary** (*.bin) + BEJ-бінар → **вихід:** JSON (UTF-8).
- **Типи:** `Set`, `Array`, `String`, `Integer`, `Boolean`, `Real`, `Enum`, `Null`.
- є Doxygen-конфіг.

> В рамках спрощення завдання: анотації не розбираються. Для демонстрації архітектури цього достатньо.


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

1/1 Test #1: test_nnint .......................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 1

## Декодування прикладу

У репозиторії є файли examples/:
- Processor_v1.bin — словник
- processor.bej — бінарний файл BEJ
- example.json — еталонний JSON

Запуск Windows PowerShell:
```
& .\build-ninja\bej2json.exe `
  -s ".\examples\Processor_v1.bin" `
  -b ".\examples\processor.bej" `
  -o ".\examples\processor_decoded.json"
  
```

bash
```
./build-ninja/bej2json \
  -s ./examples/Processor_v1.bin \
  -b ./examples/processor.bej \
  -o ./examples/processor_decoded.json
  ```


  ## Doxygen
  ```
cmake --build build-ninja --target doc
Start-Process .\docs\html\index.html
```
