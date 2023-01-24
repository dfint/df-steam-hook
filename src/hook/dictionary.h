#pragma once

class Dictionary
{
 public:
   [[nodiscard]] static Dictionary *GetSingleton()
   {
      static Dictionary singleton;
      return &singleton;
   }

   void LoadCsv(const std::string &filename, const std::string &regex_filename);
   std::optional<std::string> GetMulti(const char *key, int x, int y, bool top = false);
   std::optional<std::string> Get(const std::string &key);
   bool Exist(std::string &key);
   void Add(std::string &key, std::string &value);
   void Add(std::pair<std::string, std::string> &pair);
   size_t Size();
   void Clear();
   void CompleteBuffer();

 private:
   Dictionary() {}
   Dictionary(const Dictionary &) = delete;
   Dictionary(Dictionary &&) = delete;

   ~Dictionary() = default;

   void ReplaceAll(std::string &subject, const std::string &search, const std::string &replace);
   void EraseFrontBackBlank(std::string &str);
   std::pair<std::string, std::string> Split(const std::string &str);
   std::string Sanitize(std::string &str);

   std::optional<std::string> RegexSearch(const std::string &key);
   void SplitRegex(const std::string &str);
   void RegexRplace(std::string &str, bool on);

   std::optional<std::string> StoreStringBuffer(const char *key, int x, int y);
   void StoreTopString(const char *key, int x, int y);
   void PrepareBufferOut();
   void InitBufferVar();
   void MakeupString();

   bool is_continuous = false;
   const int line_limit = 35;
   int start_line = -1;
   int pre_line = -1;
   int end_line = 0;

   bool is_top = false;
   int top_start = -1;
   int top_pre = -1;
   int top_end = -1;

   std::unordered_map<std::string, std::string> dict;
   std::unordered_map<std::string, std::string> dict_log;
   std::unordered_map<std::string, std::string> dict_multi;
   std::vector<std::string> regex_vector;

   std::vector<std::string> key_vec;
   std::queue<std::string> value_queue;

   std::stringstream top_buffer;
};
