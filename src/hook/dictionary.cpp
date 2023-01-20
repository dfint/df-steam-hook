#include "dictionary.h"

void Dictionary::ReplaceAll(std::string& subject, const std::string& search, const std::string& replace)
{
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
}

std::string Dictionary::Sanitize(std::string& str)
{
  ReplaceAll(str, R"(\\)", "\\");
  ReplaceAll(str, R"(\t)", "\t");
  ReplaceAll(str, R"(\r)", "\r");
  ReplaceAll(str, R"(\n)", "\n");
  ReplaceAll(str, R"(")", "\"");
  return str;
}

// std::pair<std::string, std::string> Dictionary::Split(const std::string& str)
// {
//   const std::regex re(R"r(^"(.*?)","(.*?)")r");

//   std::smatch match;
//   if (regex_search(str, match, re)) {
//     auto key = std::string(match[1]);
//     auto value = std::string(match[2]);
//     return std::make_pair(Sanitize(key), Sanitize(value));
//   }
// }

std::pair<std::string, std::string> Dictionary::Split(const std::string& str)
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
  logger::info("trying to load dictionary from csv file {}", filename);
  std::ifstream file(filename);
  if (!file.is_open()) {
    logger::critical("unable to open csv file {}", filename);
    MessageBoxA(nullptr, "unable to find csv dictionary file", "dfint hook error", MB_ICONERROR);
    exit(2);
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
  logger::info("csv dictionary loaded, total lines {}", this->Size());
}

std::optional<std::string> Dictionary::Get(const std::string& key)
{
  if (key.empty()) {
    return std::nullopt;
  }
  if (this->dict.find(key) == this->dict.end()) {
    return std::nullopt;
  }
  auto value = this->dict.at(key);
  if (value.empty()) {
    return std::nullopt;
  }
  return value;
}

std::optional<std::string> Dictionary::Get(const char* key)
{
  auto len = strnlen_s(key, 1000);
  if (!key || len <= 0 || len >= 1000) {
    return std::nullopt;
  }
  if (this->dict.find(key) == this->dict.end()) {
    return std::nullopt;
  }
  auto value = this->dict.at(key);
  if (value.empty()) {
    return std::nullopt;
  }
  return value;
}

std::optional<std::string> Dictionary::Get(const char* key, size_t size)
{
  if (!key) {
    return std::nullopt;
  }

  std::unique_ptr<char[]> buf;
  if (key[size] != '\0') {
    buf = std::make_unique<char[]>(size + 1);
    strncpy(buf.get(), key, size);
    buf[size] = '\0';
    logger::debug("string {}", buf.get());
  }

  if (this->dict.find(buf ? buf.get() : key) == this->dict.end()) {
    return std::nullopt;
  }

  auto value = this->dict.at(buf ? buf.get() : key);
  if (value.empty()) {
    return std::nullopt;
  }
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

void Dictionary::Clear()
{
  this->dict.clear();
}