#include <cstdint>
#include <cstddef>
#include <cstring>
#include <iostream>

using Word = std::int32_t;

#define IS_A_INSTRUCTION(instruction)   ~(((instruction) >> 31))
#define IS_C_INSTRUCTION(instruction)   ((instruction) >> 31)

#define ADDR(instruction)               ((instruction) & static_cast<Word>(INT32_MAX))

#define USE_REGISTER_M(instruction)     ((instruction) & static_cast<Word>(0b0001000000000000))

#define ZERO_X(instruction)             ((instruction) & static_cast<Word>(0b0000100000000000))
#define NEGATE_X(instruction)           ((instruction) & static_cast<Word>(0b0000010000000000))
#define ZERO_Y(instruction)             ((instruction) & static_cast<Word>(0b0000001000000000))
#define NEGATE_Y(instruction)           ((instruction) & static_cast<Word>(0b0000000100000000))
#define ALU_ADD(instruction)            ((instruction) & static_cast<Word>(0b0000000010000000))
#define NEGATE_OUT(instruction)         ((instruction) & static_cast<Word>(0b0000000001000000))

#define DEST_A(instruction)             ((instruction) & static_cast<Word>(0b0000000000100000))
#define DEST_D(instruction)             ((instruction) & static_cast<Word>(0b0000000000010000))
#define DEST_M(instruction)             ((instruction) & static_cast<Word>(0b0000000000001000))

#define JLT(instruction)                ((instruction) & static_cast<Word>(0b0000000000000100))
#define JEQ(instruction)                ((instruction) & static_cast<Word>(0b0000000000000010))
#define JGT(instruction)                ((instruction) & static_cast<Word>(0b0000000000000001))

extern "C" {
  constexpr Word KBD = 24576;
  constexpr std::size_t ROM_SIZE = 1000000; // Change to 32 bit allows this

  Word *program = nullptr;
  Word *memory = nullptr;

  Word pc;
  Word registerA, registerD;

  void ReadProgram(char *programStr);
  void SetMemoryPtr(Word *ptr);
  void InitializeExecution();
  void Execute(std::size_t steps);
  void SetKey(Word key);
  void Reset();
  void Clear();

  void ReadProgram(char *programStr) {
    if (program == nullptr)
      program = new Word[ROM_SIZE];
    std::size_t idx = 0;
    std::size_t len = std::strlen(programStr);
    for (std::size_t i = 0; i < len; i += 33) {
      program[idx] = 0;
      for (std::size_t j = 0; j < 32; ++j)
        program[idx] = static_cast<Word>(program[idx] << 1) |
          static_cast<Word>(programStr[i + j] - '0');
      ++idx;
    }
  }

  void SetMemoryPtr(Word *ptr) {
    memory = ptr;
  }

  void InitializeExecution() {
    pc = 0;
  }

  void Execute(std::size_t steps) {
    for (std::size_t step = 0; step < steps; ++step) {
      Word instruction = program[pc++];
      Word &registerM = memory[registerA];
      if (IS_A_INSTRUCTION(instruction)) {
        registerA = ADDR(instruction);
      } else if (IS_C_INSTRUCTION(instruction)) {
        Word x = registerD;
        Word y = USE_REGISTER_M(instruction) ? registerM : registerA;
        if (ZERO_X(instruction)) x = 0;
        if (NEGATE_X(instruction)) x = ~x;
        if (ZERO_Y(instruction)) y = 0;
        if (NEGATE_Y(instruction)) y = ~y;
        Word result = ALU_ADD(instruction) ? (x + y) : (x & y);
        if (NEGATE_OUT(instruction)) result = ~result;

        if (DEST_A(instruction)) registerA = result;
        if (DEST_D(instruction)) registerD = result;
        if (DEST_M(instruction)) registerM = result;

        bool jump = (JLT(instruction) && result < 0) ||
          (JEQ(instruction) && result == 0) ||
          (JGT(instruction) && result > 0);
        if (jump) pc = registerA;
      }
    }
  }

  void SetKey(Word key) {
    memory[KBD] = key;
  }

  void Reset() {
    pc = 0;
    if (memory != nullptr) {
      for (int i = 0; i < 24577; ++i)
        memory[i] = 0;
    }
  }

  void Clear() {
    Reset();
    if (program != nullptr)
      std::fill(program, program + ROM_SIZE, 0);
  }
}
