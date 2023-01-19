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
   Dictionary()
   {
      start_xy.first = -1;
      start_xy.second = -1;
      out_xy.first = -1;
      out_xy.second = -1;
   }
   Dictionary(const Dictionary&) = delete;
   Dictionary(Dictionary&&) = delete;

   ~Dictionary() = default;

   void ReplaceAll(std::string& subject, const std::string& search, const std::string& replace);
   std::pair<std::string, std::string> Split(const std::string& str);
   std::string Sanitize(std::string& str);

   std::optional<std::string> RegexSearch(const std::string& key);
   void SplitRegex(const std::string& str);

   bool StoreStringBuffer(const char* key, int x, int y);
   void PrepareBufferOut();
   void SetStringCoord(int x, int y);
   void MakeupString();

  bool buffer_ready = false;
   const int line_limit = 54;
   int now_y = 0;
   std::pair<int, int> start_xy;
   std::pair<int, int> out_xy;


   std::unordered_map<std::string, std::string> dict;
   std::unordered_map<std::string, std::string> dict_log;
   std::set<std::string> regex_set;
   std::stringstream str_buffer;
   std::stringstream trans_buffer;
   std::stringstream out_buffer;
   std::stringstream key_buffer;
   std::vector<std::pair<std::string, int>> out_vector;
};
