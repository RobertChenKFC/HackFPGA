# Hack FPGA
This is a full stack implementation of [nand2tetris](https://www.nand2tetris.org/),
including both the software and hardware (FPGA coming soon). The implementation
details are mostly the same as those in nand2tetris, except for the fact that
the CPU is extended to 32 bits to allow for a larger program address space.

## Demo
You can use the compiler and emulator toolchain directly on this
[website](https://robertchenkfc.github.io/HackFPGA/). Simply specify all the Jack
files you would like to compile and click "Execute" to run the Jack program.

## Overview
This project contains the software development chain to implement programs for
the Hack CPU of nand2tetris, and the designs to realize the Hack CPU using
real hardware. This project contains the following components:
* `emulator`: this is the software toolchain for the Hack CPU, which includes
the Jack compiler, Jack OS, VM translator, Hack assembler and a Hack CPU emulator
into a web GUI.
* `block-diagrams`: this includes a design of the Hack CPU using
[Logisim-evolution](https://github.com/logisim-evolution/logisim-evolution).
The original plan was to implement the entire system using 74-series ICs
and other simple ICs such as parallel flash chips and SRAM chips. However, after
realizing the scale of this project, I decided to put this on hold.

## Build Instructions
### `emulator`:
* Prerequisites: [emscripten](https://emscripten.org/), [CMake](https://cmake.org/),
[GNU Make](https://www.gnu.org/software/make/)
* Build:
1. Create `build` folder
```
mkdir build
```
2. Navigate to `build` folder
```
cd build
```
3. Run `cmake` using `emcmake`
```
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
```
4. Run `make` using `emmake`
```
emmake make -j
```

## Usage
### `emulator`
Run an HTTP server at the path `emulator/src/`, and use a browser to connect to
the server, then a GUI interface will be provided for both the compilation from
the Jack language to Hack machine code and the emulated system.
