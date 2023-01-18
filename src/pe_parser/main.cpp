#include "pe_parser.hpp"
#include <format>
#include <iostream>

int main(int argc, char* argv[])
{

  auto parser = PEParser::GetSingleton();
  parser->OpenFile(argv[1]);

  std::puts(std::format("pe loaded: {}", argv[1]).data());

  for (int i = 2; i < argc; ++i) {
    auto res = parser->FindString(argv[i]);
    if (res) {
      std::puts(std::format("string: {} - 0x{:x}", argv[i], res.value()).data());
    } else {
      std::puts(std::format("string: {} - none", argv[i]).data());
    }
  }

  return EXIT_SUCCESS;
}