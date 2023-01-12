#include <cstdint>
#include <cstddef>
#include <cstring>
#include <iostream>

using Word = std::int32_t;

#define IS_A_INSTRUCTION(instruction)   ~(((instruction) >> 31))
#define IS_C_INSTRUCTION(instruction)   ((instruction) >> 31)


#define USE_REGISTER_M(instruction)     (((instruction) >> 12) & 1)

#define ZERO_X(instruction)             (((instruction) >> 11) & 1)
#define NEGATE_X(instruction)           (((instruction) >> 10) & 1)
#define ZERO_Y(instruction)             (((instruction) >>  9) & 1)
#define NEGATE_Y(instruction)           (((instruction) >>  8) & 1)
#define ALU_ADD(instruction)            (((instruction) >>  7) & 1)
#define NEGATE_OUT(instruction)         (((instruction) >>  6) & 1)

#define DEST_A(instruction)             (((instruction) >>  5) & 1)
#define DEST_D(instruction)             (((instruction) >>  4) & 1)
#define DEST_M(instruction)             (((instruction) >>  3) & 1)

#define JLT(instruction)                (((instruction) >>  2) & 1)
#define JEQ(instruction)                (((instruction) >>  1) & 1)
#define JGT(instruction)                (((instruction)      ) & 1)

extern "C" {
  constexpr Word KBD = 24576;
  constexpr std::size_t ROM_SIZE = 10000000; // Change to 32 bit allows this

  Word *program = nullptr;
  Word *memory = nullptr;

  Word pc;
  Word registerA, registerD;

  void ReadProgram(char *programStr, std::size_t len);
  void SetMemoryPtr(Word *ptr);
  void InitializeExecution();
  void Execute(std::size_t steps);
  void SetKey(Word key);
  void Reset();
  void Clear();

  void ReadProgram(char *programStr, std::size_t len) {
    if (program == nullptr)
      program = new Word[ROM_SIZE];
    std::size_t idx = 0;
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
      if (IS_A_INSTRUCTION(instruction)) {
        registerA = instruction;
      } else {
        Word x = registerD;
        Word y = USE_REGISTER_M(instruction) ?
                 memory[registerA & 0xffff] :
                 registerA;
        x = ~(-  ZERO_X(instruction)) & x;
        x =  (-NEGATE_X(instruction)) ^ x;
        y = ~(-  ZERO_Y(instruction)) & y;
        y =  (-NEGATE_Y(instruction)) ^ y;
        Word result = ALU_ADD(instruction) ? (x + y) : (x & y);
        result = (-NEGATE_OUT(instruction)) ^ result;

        if (DEST_M(instruction)) memory[registerA & 0xffff] = result;
        if (DEST_A(instruction)) registerA = result;
        if (DEST_D(instruction)) registerD = result;

        int16_t result16 = static_cast<int16_t>(result & 0xffff);
        bool jump = (JLT(instruction) && result16 <  0) ||
                    (JEQ(instruction) && result16 == 0) ||
                    (JGT(instruction) && result16 >  0);
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
