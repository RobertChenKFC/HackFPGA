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

enum class Segment {
  CONSTANT,
  LOCAL,
  ARGUMENT,
  THIS,
  THAT,
  POINTER,
  TMP,
  STATIC
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

void CompareOperation(std::fstream &outputFile, const std::string &operation, std::int16_t labelNum) {
  outputFile << "@SP" << std::endl
    << "AM=M-1" << std::endl
    << "D=M" << std::endl
    << "A=A-1" << std::endl
    << "D=M-D" << std::endl
    << "@CMP_BEGIN" << labelNum << std::endl
    << "D;" << operation << std::endl
    << "D=0" << std::endl
    << "@CMP_END" << labelNum << std::endl
    << "0;JMP" << std::endl
    << "(CMP_BEGIN" << labelNum << ")" << std::endl
    << "D=-1" << std::endl
    << "(CMP_END" << labelNum << ")" << std::endl
    << "@SP" << std::endl
    << "A=M-1" << std::endl
    << "M=D" << std::endl;
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
    default:
      // TODO: include all cases
      break;
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
    default:
      // TODO: include all cases
      break;
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

void Translate(std::fstream &outputFile, const std::string &inputFileName) {
  std::string inputClass = std::filesystem::path(inputFileName).filename().string() + ".";
  std::fstream inputFile(inputFileName, std::ios::in);

  std::string line;
  std::int16_t labelNum = 0;
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
      CompareOperation(outputFile, "JEQ", labelNum++);
    } else if (cmd == "gt") {
      CompareOperation(outputFile, "JGT", labelNum++);
    } else if (cmd == "lt") {
      CompareOperation(outputFile, "JLT", labelNum++);
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
    } else if (!std::all_of(cmd.begin(), cmd.end(),
          [](char c) { return std::isspace(static_cast<unsigned char>(c)); })) {
      throw std::runtime_error("Line " + std::to_string(lineNum) +
          ": Invalid command \"" + cmd + "\"");
    }
  }
}

int main(int argc, char **argv) {
  if (argc == 0) {
    std::cout << "Usage: " << std::endl
      << "    ./VMTranslator FILE.vm...    Translates all FILE.vm." << std::endl;
  }

  std::vector<std::string> inputFileNames;
  std::string outputFileName = "out.asm";
  for (int i = 1; i < argc; ++i) {
    if (std::strncmp(argv[i], "-o", 2) == 0) {
      if (i + 1 >= argc) {
        std::cout << "Missing filename after \"-o\"" << std::endl;
        return 0;
      }
      outputFileName = argv[i + 1];
      ++i;
    } else {
      inputFileNames.push_back(argv[i]);
    }
  }

  std::fstream outputFile(outputFileName, std::ios::out);
  for (const auto &inputFileName : inputFileNames) {
    // TODO: make it so that it all outputs to the same file
    // doing linking in the vm-translator
    try {
      Translate(outputFile, inputFileName);
    } catch (const std::exception &e) {
      std::cout << "Error while translating " << inputFileName << ':' << std::endl
        << e.what() << std::endl;
    }
  }
}

