class Dictionary
{
public:
  [[nodiscard]] static Dictionary* GetSingleton()
  {
    static Dictionary singleton;
    return &singleton;
  }

  void LoadCsv(const std::string& filename);
  std::optional<std::string> get(std::string& key);
  bool exist(std::string& key);
  void add(std::string& key, std::string& value);
  void add(std::pair<std::string, std::string>& pair);
  size_t size();

private:
  Dictionary() {}
  Dictionary(const Dictionary&) = delete;
  Dictionary(Dictionary&&) = delete;

  ~Dictionary() = default;

  std::unordered_map<std::string, std::string> dict;
};
