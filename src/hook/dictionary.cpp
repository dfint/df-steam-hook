#include "dictionary.h"

void ReplaceAll(std::string& subject, const std::string& search, const std::string& replace)
{
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
}

std::string Sanitize(std::string& str)
{
  ReplaceAll(str, R"(\\)", "\\");
  ReplaceAll(str, R"(\t)", "\t");
  ReplaceAll(str, R"(\r)", "\r");
  ReplaceAll(str, R"(\n)", "\n");
  ReplaceAll(str, R"(")", "\"");
  return str;
}

// std::optional<std::pair<std::string, std::string>> split(const std::string&
// str)
// {
//   const std::regex re(R"r(^"(.*?)","(.*?)")r");

//   std::smatch match;
//   if (regex_search(str, match, re)) {
//     return std::make_pair(match[1], match[2]);
//   }
// }

std::pair<std::string, std::string> Split(const std::string& str)
{
  std::string delimiter = "\",\"";
  auto delimiter_pos = str.find(delimiter);
  auto key = str.substr(1, delimiter_pos - 1);
  // value length = str-key-1(for fisrt ")-1(for last ")-3(for delimiter)
  auto value = str.substr(delimiter_pos + 3, str.length() - key.length() - 5);
  return std::make_pair(Sanitize(key), Sanitize(value));
}

// C code would be faster here, but we need to load just once
void Dictionary::LoadCsv(const std::string& filename)
{
  spdlog::info("trying to load dictionary from csv file {}", filename);
  std::ifstream file(filename);
  if (!file.is_open()) {
    spdlog::critical("unable to open csv file {}", filename);
    // do we need exit(2) here?
    return;
  }

  std::string line;
  int i = 0;
  while (std::getline(file, line)) {
    auto pair = Split(line);
    this->Add(pair);
  }
  file.close();
  spdlog::info("csv dictionary loaded, total lines {}", this->Size());
}

std::optional<std::string> Dictionary::Get(const std::string& key)
{
  if (this->dict.find(key) == this->dict.end()) {
    return std::nullopt;
  }
  auto value = this->dict.at(key);
  return value;
}

bool Dictionary::Exist(std::string& key)
{
  return this->dict.find(key) != this->dict.end();
}

void Dictionary::Add(std::string& key, std::string& value)
{
  this->dict.emplace(std::make_pair(key, value));
}

void Dictionary::Add(std::pair<std::string, std::string>& pair)
{
  this->dict.emplace(pair);
}

size_t Dictionary::Size()
{
  return this->dict.size();
}
