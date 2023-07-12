#pragma once

class Dictionary {
public:
  [[nodiscard]] static Dictionary* GetSingleton() {
    static Dictionary singleton;
    return &singleton;
  }

  void LoadCsv(const std::string& filename);
  std::optional<std::string> Get(const std::string& key);
  std::optional<std::string> Get(const char* key);
  std::optional<std::string> Get(const char* key, size_t size);
  bool Exist(std::string& key);
  void Add(std::string& key, std::string& value);
  void Add(std::pair<std::string, std::string>& pair);
  size_t Size();
  void Clear();

private:
  Dictionary() {}
  Dictionary(const Dictionary&) = delete;
  Dictionary(Dictionary&&) = delete;

  ~Dictionary() = default;

  void ReplaceAll(std::string& subject, const std::string& search, const std::string& replace);
  std::pair<std::string, std::string> Split(const std::string& str);
  std::string Sanitize(std::string& str);

  std::unordered_map<std::string, std::string> dict;
};
