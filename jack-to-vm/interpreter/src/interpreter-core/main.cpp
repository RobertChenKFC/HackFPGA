#include "core/tokenizer.h"
#include "core/parser.h"
#include "core/vmcommand.h"

#include <string>
#include <exception>
#include <iostream>
#include <emscripten.h>

extern "C" {
  VMCommands vmCommands;

  bool CompileFile(char *inputFileName, char *input);
  void SetMemoryPtr(std::int16_t *ptr);
  bool InitializeExecution();
  bool Execute(std::size_t steps);
  void SetKey(std::int16_t key);
  void Reset();
  void Clear();

  bool CompileFile(char *inputFileName, char *input) {
    try {
      Tokenizer::init();
      Parser parser(inputFileName, input);
      vmCommands.add(parser.toVMCommands());

      EM_ASM_({
        console.log('Compiled ' + Pointer_stringify($0));
      }, inputFileName);

      return true;
    } catch (const std::exception &e) {
      EM_ASM_({
        console.log(Pointer_stringify($0) + ', ' + Pointer_stringify($1));
      }, inputFileName, e.what());

      return false;
    }
  }

  void SetMemoryPtr(std::int16_t *ptr) {
    try {
      vmCommands.setMemoryPtr(ptr);
    } catch (const std::exception &e) {
      EM_ASM_({
        console.log(Pointer_stringify($0));
      }, e.what());
    }
  }

  bool InitializeExecution() {
    try {
      VMCommands::init();
      vmCommands.add(VMCommands("jack-os/Array.vm"));
      vmCommands.add(VMCommands("jack-os/Keyboard.vm"));
      vmCommands.add(VMCommands("jack-os/Math.vm"));
      vmCommands.add(VMCommands("jack-os/Memory.vm"));
      vmCommands.add(VMCommands("jack-os/Output.vm"));
      vmCommands.add(VMCommands("jack-os/Screen.vm"));
      vmCommands.add(VMCommands("jack-os/String.vm"));
      vmCommands.add(VMCommands("jack-os/Sys.vm"));
      vmCommands.initExecution();

      return true;
    } catch (const std::exception &e){
      EM_ASM_({
        console.log(Pointer_stringify($0));
      }, e.what());

      return false;
    }
  }

  bool Execute(std::size_t steps) {
    bool done = false;
    try {
      done = vmCommands.execute(steps);
    } catch (const std::exception &e) {
      EM_ASM_({
        console.log(Pointer_stringify($0));
      }, e.what());
    }
    return done;
  }

  void SetKey(std::int16_t key) {
    vmCommands.setKey(key);
  }

  void Reset() {
    vmCommands.reset();
  }

  void Clear() {
    vmCommands.clear();
  }

  int main() {

  }
}

