#include "dictionary.h"

void Dictionary::ReplaceAll(std::string& subject, const std::string& search, const std::string& replace) {
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
}

std::string Dictionary::Sanitize(std::string& str) {
  ReplaceAll(str, R"("")", "\"");
  return str;
}

std::pair<std::string, std::string> Dictionary::Split(const std::string& str) {
  std::string delimiter = "\",\"";
  auto delimiter_pos = str.find(delimiter);
  auto key = str.substr(1, delimiter_pos - 1);
  auto value = str.substr(delimiter_pos + 3, str.length() - key.length() - 5);
  return std::make_pair(Sanitize(key), Sanitize(value));
}

// C code would be faster here, but we need to load just once
void Dictionary::LoadCsv(const std::string& filename) {
  logger::info("trying to load dictionary from csv file {}", filename);
  std::ifstream file(filename);
  if (!file.is_open()) {
    logger::critical("unable to open csv file {}", filename);
    MessageBoxA(nullptr, "unable to find csv dictionary file", "dfint hook error", MB_ICONERROR);
    exit(2);
    return;
  }

  std::string line;
  while (std::getline(file, line)) {
    auto pair = Split(line);
    this->Add(pair);
  }
  file.close();
  logger::info("csv dictionary loaded, total lines {}", this->Size());
}

std::optional<std::string> Dictionary::Get(const std::string& key) {
  if (key.empty()) {
    return std::nullopt;
  }
  if (auto it = this->dict.find(key); it != this->dict.end()) {
    if (auto value = it->second; !value.empty()) {
      return value;
    }
  }
  return std::nullopt;
}

std::optional<std::string> Dictionary::Get(const char* key) {
  auto len = strnlen_s(key, 1000);
  if (!key || len <= 0 || len >= 1000) {
    return std::nullopt;
  }
  if (auto it = this->dict.find(key); it != this->dict.end()) {
    if (auto value = it->second; !value.empty()) {
      return value;
    }
  }
  return std::nullopt;
}

std::optional<std::string> Dictionary::Get(const char* key, size_t size) {
  if (!key) {
    return std::nullopt;
  }
  std::unique_ptr<char[]> buf;
  if (key[size] != '\0') {
    buf = std::make_unique<char[]>(size + 1);
    strncpy(buf.get(), key, size);
    buf[size] = '\0';
  }
  if (auto it = this->dict.find(buf ? buf.get() : key); it != this->dict.end()) {
    if (auto value = it->second; !value.empty()) {
      return value;
    }
  }
  return std::nullopt;
}

bool Dictionary::Exist(std::string& key) {
  return this->dict.contains(key);
}

void Dictionary::Add(std::string& key, std::string& value) {
  this->dict.emplace(std::make_pair(key, value));
}

void Dictionary::Add(std::pair<std::string, std::string>& pair) {
  this->dict.emplace(pair);
}

size_t Dictionary::Size() {
  return this->dict.size();
}

void Dictionary::Clear() {
  this->dict.clear();
}