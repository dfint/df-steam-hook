class Dictionary
{
public:
  [[nodiscard]] static Dictionary* GetSingleton()
  {
    static Dictionary singleton;
    return &singleton;
  }

  void LoadCsv(const std::string& filename);
  std::optional<std::string> Get(std::string& key);
  bool Exist(std::string& key);
  void Add(std::string& key, std::string& value);
  void Add(std::pair<std::string, std::string>& pair);
  size_t Size();

private:
  Dictionary() {}
  Dictionary(const Dictionary&) = delete;
  Dictionary(Dictionary&&) = delete;

  ~Dictionary() = default;

  std::unordered_map<std::string, std::string> dict;
};
