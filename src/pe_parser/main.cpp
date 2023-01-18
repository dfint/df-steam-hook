#include "pe_parser.hpp"
#include <format>
#include <iostream>

int main(int argc, char* argv[])
{

  auto parser = PEParser::GetSingleton();
  parser->OpenFile("E:\\Games\\Dwarf Fortress Steam 50.05\\Dwarf Fortress.exe");

  for (int i = 1; i < argc; ++i) {
    auto res = parser->FindString(argv[i]);
    if (res) {
      std::puts(std::format("{} - {:x}", argv[i], res.value()).data());
    } else {
      std::puts(std::format("{} - none", argv[i]).data());
    }
  }

  return EXIT_SUCCESS;
}