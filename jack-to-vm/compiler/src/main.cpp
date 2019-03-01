#include "core/tokenizer.h"
#include "core/parser.h"

#include <string>
#include <iostream>
#include <filesystem>

enum class Option {
  TOKENIZE,
  PARSE,
  COMPILE
};

int main(int argc, char **argv) {
  std::vector<std::string> inputFileNames;
  Option option = Option::COMPILE;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg[0] == '-' && arg[1] == '-') {
      if (arg == "--tokenize") {
        option = Option::TOKENIZE;
      } else if (arg == "--parse") {
        option = Option::PARSE;
      } else if (arg == "--help") {
        std::cout << "Usage: " << std::endl
          << "    ./JackCompiler [options]                 Compiles all .jack files in the current" << std::endl
          << "                                             working directory." << std::endl
          << "    ./JackCompiler [options] DIRECTORY       Compiles all .jack files in DIRECTORY." << std::endl
          << "    ./JackCompiler [options] FILE.jack...    Compiles all FILE.jack." << std::endl
          << "Options: " << std::endl
          << "    --help                                   Display this information." << std::endl
          << "    --tokenize                               Tokenizes FILE.jack to FILET.xml." << std::endl
          << "    --parse                                  Parses FILE.jack to FILE.xml." << std::endl;
      } else {
        std::cout << "Invalid option \"" << arg  << "\"" << std::endl;
        return 0;
      }
    }
    else inputFileNames.push_back(std::move(arg));
  }

  if (inputFileNames.empty()) {
    for (const auto &file : std::filesystem::directory_iterator(std::filesystem::current_path())) {
      if (file.path().extension() == ".jack")
        inputFileNames.push_back(file.path().string());
    }
  } else if (inputFileNames.size() == 1) {
    std::filesystem::directory_entry directory(inputFileNames.front());
    if (directory.is_directory()) {
      inputFileNames.clear();
      for (const auto &file : std::filesystem::directory_iterator(directory)) {
        if (file.path().extension() == ".jack")
          inputFileNames.push_back(file.path().string());
      }
    } else {
      for (const auto &inputFileName : inputFileNames) {
        if (std::filesystem::path(inputFileName).extension() != ".jack") {
          std::cout << "Invalid file " << inputFileName << std::endl;
          return 0;
        }
      }
    }
  }

  Tokenizer::init();
  for (const auto &inputFileName : inputFileNames) {
    try {
      switch (option) {
        case Option::TOKENIZE: {
          Tokenizer tokenizer(inputFileName);
          std::string outputFileName = inputFileName.substr(0, inputFileName.find_last_of('.')) + "T.xml";
          tokenizer.toXML(outputFileName);
          break;
        }
        case Option::PARSE: {
          Parser parser(inputFileName);
          std::string outputFileName = inputFileName.substr(0, inputFileName.find_last_of('.')) + ".xml";
          parser.toXML(outputFileName);
          break;
        }
        case Option::COMPILE: {
          Parser parser(inputFileName);
          std::string outputFileName = inputFileName.substr(0, inputFileName.find_last_of('.')) + ".vm";
          parser.toVMCode(outputFileName);
          break;
        }
        default:
          break;
      }
    } catch (const std::exception &e) {
      std::cout << inputFileName << ", " << e.what() << std::endl;
    }
  }
}

