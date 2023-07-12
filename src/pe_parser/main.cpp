#include "pe_parser.hpp"
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

/*
 * first argument - path to Dwarf Fortress.exe
 * second argument - file with strings to translate
 */

int main(int argc, char* argv[]) {
  std::ifstream file(argv[2]);
  if (!file.is_open()) {
    std::puts(std::format("unable to open strings file {}", argv[2]).data());
    return EXIT_FAILURE;
  }

  auto parser = PEParser::GetSingleton();
  parser->OpenFile(argv[1]);
  std::puts(std::format("pe loaded: {}", argv[1]).data());

  std::ofstream output(std::format("{}.out.txt ", argv[2]).data());
  std::string line;
  while (std::getline(file, line)) {
    auto res = parser->FindString(line);
    if (res) {
      output << std::format("0x{:x}", res.value()).data() << "\n";
      std::cout << std::format("0x{:x}", res.value()).data() << " >> " << line << "\n";
    } else {
      output << "0x0\n";
      std::cout << "nothing found >> " << line << "\n";
    }
  }

  output.close();
  file.close();

  return EXIT_SUCCESS;
}