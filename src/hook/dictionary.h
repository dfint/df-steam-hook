#pragma once

class Dictionary
{
  public:
   [[nodiscard]] static Dictionary* GetSingleton()
   {
      static Dictionary singleton;
      return &singleton;
   }

   void LoadCsv(const std::string& filename, const std::string& regex_filename);
   std::optional<std::string> GetMulti(const char* key, int x, int y);
   std::optional<std::string> Get(const std::string& key);
   bool Exist(std::string& key);
   void Add(std::string& key, std::string& value);
   void Add(std::pair<std::string, std::string>& pair);
   size_t Size();
   void Clear();
   void CompleteBuffer();

  private:
   Dictionary() {}
   Dictionary(const Dictionary&) = delete;
   Dictionary(Dictionary&&) = delete;

   ~Dictionary() = default;

   void ReplaceAll(std::string& subject, const std::string& search, const std::string& replace);
   std::pair<std::string, std::string> Split(const std::string& str);
   std::string Sanitize(std::string& str);

   std::optional<std::string> RegexSearch(const std::string& key);
   void SplitRegex(const std::string& str);

   std::optional<std::string> StoreStringBuffer(const char* key, int x, int y);
   void PrepareBufferOut();
   void InitBufferVar();
   void MakeupString();

   bool is_ready = false;
   const int line_limit = 40;
   int start_y = -1;
   int now_y = 0;

   std::unordered_map<std::string, std::string> dict;
   std::unordered_map<std::string, std::string> dict_log;
   std::unordered_map<std::string, std::string> dict_multi;
   std::unordered_set<std::string> regex_set;
   
   std::vector<std::string> key_vec;
   std::queue<std::string> value_queue;
};
