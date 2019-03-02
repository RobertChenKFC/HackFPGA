#include <cstdint>
#include <cstddef>
#include <cstring>
#include <iostream>

#define IS_A_INSTRUCTION(instruction)   ~(((instruction) >> 15))
#define IS_C_INSTRUCTION(instruction)   ((instruction) >> 15)

#define ADDR(instruction)               ((instruction) & static_cast<std::int16_t>(0b0111111111111111))

#define USE_REGISTER_M(instruction)     ((instruction) & static_cast<std::int16_t>(0b0001000000000000))

#define ZERO_X(instruction)             ((instruction) & static_cast<std::int16_t>(0b0000100000000000))
#define NEGATE_X(instruction)           ((instruction) & static_cast<std::int16_t>(0b0000010000000000))
#define ZERO_Y(instruction)             ((instruction) & static_cast<std::int16_t>(0b0000001000000000))
#define NEGATE_Y(instruction)           ((instruction) & static_cast<std::int16_t>(0b0000000100000000))
#define ALU_ADD(instruction)            ((instruction) & static_cast<std::int16_t>(0b0000000010000000))
#define NEGATE_OUT(instruction)         ((instruction) & static_cast<std::int16_t>(0b0000000001000000))

#define DEST_A(instruction)             ((instruction) & static_cast<std::int16_t>(0b0000000000100000))
#define DEST_D(instruction)             ((instruction) & static_cast<std::int16_t>(0b0000000000010000))
#define DEST_M(instruction)             ((instruction) & static_cast<std::int16_t>(0b0000000000001000))

#define JLT(instruction)                ((instruction) & static_cast<std::int16_t>(0b0000000000000100))
#define JEQ(instruction)                ((instruction) & static_cast<std::int16_t>(0b0000000000000010))
#define JGT(instruction)                ((instruction) & static_cast<std::int16_t>(0b0000000000000001))

extern "C" {
  constexpr std::int16_t KBD = 24576;

  std::int16_t *program = nullptr;
  std::int16_t *memory = nullptr;

  std::int16_t pc;
  std::int16_t registerA, registerD;

  void ReadProgram(char *programStr);
  void SetMemoryPtr(std::int16_t *ptr);
  void InitializeExecution();
  void Execute(std::size_t steps);
  void SetKey(std::int16_t key);
  void Reset();
  void Clear();

  void ReadProgram(char *programStr) {
    if (program == nullptr) program = new std::int16_t[32768];
    std::size_t idx = 0;
    std::size_t len = std::strlen(programStr);
    for (std::size_t i = 0; i < len; i += 17) {
      program[idx] = 0;
      for (std::size_t j = 0; j < 16; ++j)
        program[idx] = static_cast<std::int16_t>(program[idx] << 1) |
          static_cast<std::int16_t>(programStr[i + j] - '0');
      ++idx;
    }
  }

  void SetMemoryPtr(std::int16_t *ptr) {
    memory = ptr;
  }

  void InitializeExecution() {
    pc = 0;
  }

  void Execute(std::size_t steps) {
    for (std::size_t step = 0; step < steps; ++step) {
      std::int16_t instruction = program[pc++];
      std::int16_t &registerM = memory[registerA];
      if (IS_A_INSTRUCTION(instruction)) {
        registerA = ADDR(instruction);
      } else if (IS_C_INSTRUCTION(instruction)) {
        std::int16_t x = registerD;
        std::int16_t y = USE_REGISTER_M(instruction) ? registerM : registerA;
        if (ZERO_X(instruction)) x = 0;
        if (NEGATE_X(instruction)) x = ~x;
        if (ZERO_Y(instruction)) y = 0;
        if (NEGATE_Y(instruction)) y = ~y;
        std::int16_t result = ALU_ADD(instruction) ? (x + y) : (x & y);
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

  void SetKey(std::int16_t key) {
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
    if (program != nullptr) {
      for (int i = 0; i < 32768; ++i)
        program[i] = 0;
    }
  }
}
