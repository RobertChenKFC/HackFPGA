#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <cctype>

std::string BitString(std::int16_t x, int bits = 16) {
  std::string s;
  while (bits--)
    s.push_back(static_cast<char>((x >> bits) & 1) + '0');
  return s;
}

std::unordered_map<std::string, std::int16_t> baseSymbolTable;
std::unordered_map<std::string, std::string> instruction;
void Initialize() {
  baseSymbolTable["SP"] =     "000000000000000";
  baseSymbolTable["LCL"] =    "000000000000001";
  baseSymbolTable["ARG"] =    "000000000000010";
  baseSymbolTable["THIS"] =   "000000000000011";
  baseSymbolTable["THAT"] =   "000000000000100";
  baseSymbolTable["R0"] =     "000000000000000";
  baseSymbolTable["R1"] =     "000000000000001";
  baseSymbolTable["R2"] =     "000000000000010";
  baseSymbolTable["R3"] =     "000000000000011";
  baseSymbolTable["R4"] =     "000000000000100";
  baseSymbolTable["R5"] =     "000000000000101";
  baseSymbolTable["R6"] =     "000000000000110";
  baseSymbolTable["R7"] =     "000000000000111";
  baseSymbolTable["R8"] =     "000000000001000";
  baseSymbolTable["R9"] =     "000000000001001";
  baseSymbolTable["R10"] =    "000000000001010";
  baseSymbolTable["R11"] =    "000000000001011";
  baseSymbolTable["R12"] =    "000000000001100";
  baseSymbolTable["R13"] =    "000000000001101";
  baseSymbolTable["R14"] =    "000000000001110";
  baseSymbolTable["R15"] =    "000000000001111";
  baseSymbolTable["SCREEN"] = "100000000000000";
  baseSymbolTable["KBD"] =    "110000000000000";
}

bool IsNumber(const std::string &str) {
  for (const auto &c : str)
    if (!std::isdigit(c)) return false;
  return true;
}

void Assemble(const std::string &inputFileName, const std::string &outputFileName) {
  std::fstream inputFile(inputFileName, std::ios::in);
  std::fstream outputFile(outputFileName, std::ios::out);

  std::string line;
  std::unordered_map<std::string, std::int16_t> labelTable;
  std::unordered_map<std::string, std::int16_t> symbolTable(baseSymbolTable);
  std::int16_t asmLineNumber = 0, hackLineNumber = 0;
  std::int16_t symbolNumber = 16;
  while (std::getline(inputFile, line)) {
    ++asmLineNumber;
    if (line.front() == '(') {
      // Label
    } else if (line.front() == '@') {
      // A instruction
      ++hackLineNumber;
      auto address = line.substr(1);
      if (!IsNumber(address) && symbolTable.find(address) == symbolTable.end())
        symbolTable[address] = symbolNumber++;
    } else {
      // C instruction
      ++hackLineNumber;

    }
  }
}

int main(int argc, char **argv) {
  if (argc == 0) {
    std::cout << "Usage: " << std::endl
      << "    ./JackAssembler FILE.asm...    Assembles all FILE.asm." << std::endl;
  }
  for (int i = 1; i < argc; ++i) {
    std::string inputFileName = argv[i];
    std::string outputFileName = inputFileName.substr(0, inputFileName.find_last_of('.')) + ".hack";
    try {
      Assemble(inputFileName, outputFileName);
    } catch (const std::exception &e) {
      std::cout << "Error while assembling " << inputFileName << std::endl
        << e.what() << std::endl;
    }
  }
}

