#include <cctype>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <vector>

using Word = std::int32_t;

enum class Segment {
  CONSTANT,
  LOCAL,
  ARGUMENT,
  THIS,
  THAT,
  POINTER,
  TMP,
  STATIC,
  VALUE,
  FRAME
};

void UnaryOperation(std::fstream &outputFile, char operation) {
  outputFile << "@SP" << std::endl
    << "A=M-1" << std::endl
    << "M=" << operation << "M" << std::endl;
}

void BinaryOperation(std::fstream &outputFile, char operation) {
  outputFile << "@SP" << std::endl
    << "AM=M-1" << std::endl
    << "D=M" << std::endl
    << "A=A-1" << std::endl;
  if (operation == '-')
    outputFile << "M=M-D" << std::endl;
  else
    outputFile << "M=D" << operation << "M" << std::endl;
}

Word labelNum = 0;
void CompareOperation(std::fstream &outputFile, const std::string &operation) {
  outputFile << "@SP" << std::endl
    << "AM=M-1" << std::endl
    << "D=M" << std::endl
    << "A=A-1" << std::endl
    << "D=M-D" << std::endl
    << "@TRANSLATOR_CMP_BEGIN" << labelNum << std::endl
    << "D;" << operation << std::endl
    << "D=0" << std::endl
    << "@TRANSLATOR_CMP_END" << labelNum << std::endl
    << "0;JMP" << std::endl
    << "(TRANSLATOR_CMP_BEGIN" << labelNum << ")" << std::endl
    << "D=-1" << std::endl
    << "(TRANSLATOR_CMP_END" << labelNum << ")" << std::endl
    << "@SP" << std::endl
    << "A=M-1" << std::endl
    << "M=D" << std::endl;
  ++labelNum;
}

void Push(std::fstream &outputFile, Segment segment, const std::string &value) {
  std::string segmentAddress;
  switch (segment) {
    case Segment::CONSTANT:
      outputFile << "@" << value << std::endl
        << "D=A" << std::endl
        << "@SP" << std::endl
        << "A=M" << std::endl
        << "M=D" << std::endl
        << "@SP" << std::endl
        << "M=M+1" << std::endl;
      return;
    case Segment::LOCAL:
      segmentAddress = "LCL";
      break;
    case Segment::ARGUMENT:
      segmentAddress = "ARG";
      break;
    case Segment::THIS:
      segmentAddress = "THIS";
      break;
    case Segment::THAT:
      segmentAddress = "THAT";
      break;
    case Segment::TMP:
      outputFile << "@" << 5 + std::stoi(value) << std::endl
        << "D=M" << std::endl
        << "@SP" << std::endl
        << "A=M" << std::endl
        << "M=D" << std::endl
        << "@SP" << std::endl
        << "M=M+1" << std::endl;
      return;
    case Segment::POINTER:
      outputFile << "@" << 3 + std::stoi(value) << std::endl
        << "D=M" << std::endl
        << "@SP" << std::endl
        << "A=M" << std::endl
        << "M=D" << std::endl
        << "@SP" << std::endl
        << "M=M+1" << std::endl;
      return;
    case Segment::STATIC:
      outputFile << "@" << value << std::endl
        << "D=M" << std::endl
        << "@SP" << std::endl
        << "A=M" << std::endl
        << "M=D" << std::endl
        << "@SP" << std::endl
        << "M=M+1" << std::endl;
      return;
    case Segment::VALUE:
      outputFile << "@" << value << std::endl
        << "D=M" << std::endl
        << "@SP" << std::endl
        << "A=M" << std::endl
        << "M=D" << std::endl
        << "@SP" << std::endl
        << "M=M+1" << std::endl;
      return;
    default:
      return;
  }

  outputFile << "@" << segmentAddress << std::endl
    << "D=M" << std::endl
    << "@" << value << std::endl
    << "A=D+A" << std::endl
    << "D=M" << std::endl
    << "@SP" << std::endl
    << "A=M" << std::endl
    << "M=D" << std::endl
    << "@SP" << std::endl
    << "M=M+1" << std::endl;
}

void Pop(std::fstream &outputFile, Segment segment, const std::string &value) {
  std::string segmentAddress;
  switch (segment) {
    case Segment::LOCAL:
      segmentAddress = "LCL";
      break;
    case Segment::ARGUMENT:
      segmentAddress = "ARG";
      break;
    case Segment::THIS:
      segmentAddress = "THIS";
      break;
    case Segment::THAT:
      segmentAddress = "THAT";
      break;
    case Segment::TMP:
      outputFile << "@SP" << std::endl
        << "AM=M-1" << std::endl
        << "D=M" << std::endl
        << "@" << 5 + std::stoi(value) << std::endl
        << "M=D" << std::endl;
      return;
    case Segment::POINTER:
      outputFile << "@SP" << std::endl
        << "AM=M-1" << std::endl
        << "D=M" << std::endl
        << "@" << 3 + std::stoi(value) << std::endl
        << "M=D" << std::endl;
      return;
    case Segment::STATIC:
      outputFile << "@SP" << std::endl
        << "AM=M-1" << std::endl
        << "D=M" << std::endl
        << "@" << value << std::endl
        << "M=D" << std::endl;
      return;
    case Segment::VALUE:
      outputFile << "@SP" << std::endl
        << "AM=M-1" << std::endl
        << "D=M" << std::endl
        << "@" << value << std::endl
        << "M=D" << std::endl;
      return;
    case Segment::FRAME:
      outputFile << "@TRANSLATOR_frame" << std::endl
        << "AM=M-1" << std::endl
        << "D=M" << std::endl
        << "@" << value << std::endl
        << "M=D" << std::endl;
      return;
    default:
      return;
  }

  outputFile << "@" << value << std::endl
    << "D=A" << std::endl
    << "@" << segmentAddress << std::endl
    << "M=D+M" << std::endl
    << "@SP" << std::endl
    << "AM=M-1" << std::endl
    << "D=M" << std::endl
    << "@" << segmentAddress << std::endl
    << "A=M" << std::endl
    << "M=D" << std::endl
    << "@" << value << std::endl
    << "D=A" << std::endl
    << "@" << segmentAddress << std::endl
    << "M=M-D" << std::endl;
}

Word returnNum = 0;
void Call(std::fstream &outputFile, const std::string &label, Word nArgs) {
  std::string returnLabel = "TRANSLATOR_RETURN" + std::to_string(returnNum++);
  Push(outputFile, Segment::CONSTANT, returnLabel);
  Push(outputFile, Segment::VALUE, "LCL");
  Push(outputFile, Segment::VALUE, "ARG");
  Push(outputFile, Segment::VALUE, "THIS");
  Push(outputFile, Segment::VALUE, "THAT");
  outputFile << "@SP" << std::endl
    << "D=M" << std::endl
    << "@LCL" << std::endl
    << "M=D" << std::endl
    << "@" << 5 + nArgs << std::endl
    << "D=D-A" << std::endl
    << "@ARG" << std::endl
    << "M=D" << std::endl
    << "@" << label << std::endl
    << "0;JMP" << std::endl
    << "(" << returnLabel << ")" << std::endl;
}

void Return(std::fstream &outputFile) {
  outputFile << "@LCL" << std::endl
    << "D=M" << std::endl
    << "@TRANSLATOR_frame" << std::endl
    << "M=D" << std::endl
    << "@5" << std::endl
    << "A=D-A" << std::endl
    << "D=M" << std::endl
    << "@TRANSLATOR_retAddr" << std::endl
    << "M=D" << std::endl;
  Pop(outputFile, Segment::ARGUMENT, "0");
  outputFile << "@ARG" << std::endl
    << "D=M" << std::endl
    << "@SP" << std::endl
    << "M=D+1" << std::endl;
  Pop(outputFile, Segment::FRAME, "THAT");
  Pop(outputFile, Segment::FRAME, "THIS");
  Pop(outputFile, Segment::FRAME, "ARG");
  Pop(outputFile, Segment::FRAME, "LCL");
  outputFile << "@TRANSLATOR_retAddr" << std::endl
    << "A=M" << std::endl
    << "0;JMP" << std::endl;
}

void Translate(std::fstream &outputFile, const std::string &inputFileName) {
  std::string inputClass = std::filesystem::path(inputFileName).filename().string() + ".";
  std::fstream inputFile(inputFileName, std::ios::in);

  std::string line;
  std::size_t lineNum = 0;
  while (std::getline(inputFile, line)) {
    ++lineNum;
    if (line.front() == '/') continue;

    std::stringstream input(line);
    std::string cmd;
    input >> cmd;

    if (cmd == "add") {
      BinaryOperation(outputFile, '+');
    } else if (cmd == "sub") {
      BinaryOperation(outputFile, '-');
    } else if (cmd == "neg") {
      UnaryOperation(outputFile, '-');
    } else if (cmd == "eq") {
      CompareOperation(outputFile, "JEQ");
    } else if (cmd == "gt") {
      CompareOperation(outputFile, "JGT");
    } else if (cmd == "lt") {
      CompareOperation(outputFile, "JLT");
    } else if (cmd == "and") {
      BinaryOperation(outputFile, '&');
    } else if (cmd == "or") {
      BinaryOperation(outputFile, '|');
    } else if (cmd == "not") {
      UnaryOperation(outputFile, '!');
    } else if (cmd == "push") {
      std::string segment, value;
      input >> segment >> value;

      if (segment == "constant") {
        Push(outputFile, Segment::CONSTANT, value);
      } else if (segment == "local") {
        Push(outputFile, Segment::LOCAL, value);
      } else if (segment == "argument") {
        Push(outputFile, Segment::ARGUMENT, value);
      } else if (segment == "this") {
        Push(outputFile, Segment::THIS, value);
      } else if (segment == "that") {
        Push(outputFile, Segment::THAT, value);
      } else if (segment == "temp") {
        Push(outputFile, Segment::TMP, value);
      } else if (segment == "pointer") {
        Push(outputFile, Segment::POINTER, value);
      } else if (segment == "static") {
        Push(outputFile, Segment::STATIC, inputClass + value);
      } else {
        throw std::runtime_error("Line " + std::to_string(lineNum) +
            ": Pushing into invalid segment \"" + cmd + "\"");
      }
    } else if (cmd == "pop") {
      std::string segment, value;
      input >> segment >> value;

      if (segment == "local") {
        Pop(outputFile, Segment::LOCAL, value);
      } else if (segment == "argument") {
        Pop(outputFile, Segment::ARGUMENT, value);
      } else if (segment == "this") {
        Pop(outputFile, Segment::THIS, value);
      } else if (segment == "that") {
        Pop(outputFile, Segment::THAT, value);
      } else if (segment == "temp") {
        Pop(outputFile, Segment::TMP, value);
      } else if (segment == "pointer") {
        Pop(outputFile, Segment::POINTER, value);
      } else if (segment == "static") {
        Pop(outputFile, Segment::STATIC, inputClass + value);
      } else {
        throw std::runtime_error("Line " + std::to_string(lineNum) +
            ": Popping out of invalid segment \"" + cmd + "\"");
      }
    } else if (cmd == "label") {
      std::string label;
      input >> label;
      outputFile << "(" << label << ")" << std::endl;
    } else if (cmd == "goto") {
      std::string label;
      input >> label;
      outputFile << "@" << label << std::endl
        << "0;JMP" << std::endl;
    } else if (cmd == "if-goto") {
      std::string label;
      input >> label;
      outputFile << "@SP" << std::endl
        << "AM=M-1" << std::endl
        << "D=M" << std::endl
        << "@" << label << std::endl
        << "D;JNE" << std::endl;
    } else if (cmd == "function") {
      std::string label;
      Word nVars;
      input >> label >> nVars;
      outputFile << "(" << label << ")" << std::endl;
      while (nVars--) Push(outputFile, Segment::CONSTANT, "0");
    } else if (cmd == "call") {
      std::string label;
      Word nArgs;
      input >> label >> nArgs;
      Call(outputFile, label, nArgs);
    } else if (cmd == "return") {
      Return(outputFile);
    } else if (!std::all_of(cmd.begin(), cmd.end(),
          [](char c) { return std::isspace(static_cast<unsigned char>(c)); })) {
      throw std::runtime_error("Line " + std::to_string(lineNum) +
          ": Invalid command \"" + cmd + "\"");
    }
  }
}

void Bootstrap(std::fstream &outputFile) {
  outputFile << "@256" << std::endl
    << "D=A" << std::endl
    << "@SP" << std::endl
    << "M=D" << std::endl;
  Call(outputFile, "Sys.init", 0);
}

void PrintHelp() {
  std::cout << "Usage: " << std::endl
    << "    ./VMTranslator FILE.vm...    Translates all FILE.vm." << std::endl
    << "Options: " << std::endl
    << "    --help                       Display this information." << std::endl
    << "    --no-bootstrap               Translate vm-file without bootstrap code." << std::endl;
}

int main(int argc, char **argv) {
  // TODO: add options for no os and no bootstrap
  std::vector<std::string> inputFileNames;
  std::string outputFileName = "out.asm";
  bool bootstrap = true;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg.front() == '-') {
      if (arg == "--help") {
        PrintHelp();
        return 0;
      } else if (arg == "-o") {
        if (i + 1 >= argc) {
          std::cout << "Missing filename after \"-o\"" << std::endl;
          return 0;
        }
        outputFileName = argv[i + 1];
        ++i;
      } else if (arg == "--no-bootstrap") {
        bootstrap = false;
      } else {
        std::cout << "Unknown option \"" << arg << "\"" << std::endl;
      }
    } else {
      inputFileNames.push_back(argv[i]);
    }
  }
  if (inputFileNames.empty()) {
    PrintHelp();
    return 0;
  }

  std::fstream outputFile(outputFileName, std::ios::out);
  if (bootstrap) Bootstrap(outputFile);
  for (const auto &inputFileName : inputFileNames) {
    try {
      Translate(outputFile, inputFileName);
    } catch (const std::exception &e) {
      std::cout << "Error while translating " << inputFileName << ':' << std::endl
        << e.what() << std::endl;
    }
  }
}

