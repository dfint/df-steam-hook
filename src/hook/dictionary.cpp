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

void Dictionary::SplitRegex(const std::string& str)
{
   const std::regex re("(.*)=(.*)");

   std::smatch match;
   if (std::regex_search(str, match, re)) {
      std::string key = match[1];
      std::string arg = match[2];
      key = Sanitize(key);
      arg = Sanitize(arg);
      ReplaceAll(arg, "\\", "$");
      this->Add(key, arg);

      if (key.find("\\s") != std::string::npos) {
         ReplaceAll(key, "\\s", "(.*)");
         this->regex_set.emplace(key);
      }
   }
}

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
void Dictionary::LoadCsv(const std::string& filename, const std::string& regex_filename)
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

   std::ifstream regex_file;
   regex_file.open(regex_filename);
   if (!regex_file.is_open()) {
      logger::critical("unable to open regex file {}", regex_filename);
      MessageBoxA(nullptr, "unable to open regex file", "dfint hook error", MB_ICONERROR);
      exit(2);
      // do we need exit(2) here?
      return;
   }

   while (std::getline(regex_file, line)) {
      SplitRegex(line);
   }
   regex_file.close();
   logger::info("add regex in csv dictionary, total lines {}", this->Size());
}

std::optional<std::string> Dictionary::RegexSearch(const std::string& key)
{
   std::smatch match;
   for (const auto regex_string : this->regex_set) {
      std::regex r(regex_string);
      spdlog::debug("#regex string {}", regex_string);
      if (std::regex_match(key, match, r) && match.size() > 1) {
         spdlog::debug("#FIND {}", key);

         std::string find(regex_string);
         ReplaceAll(find, "(.*)", "\\s");

         std::string result;
         auto it = this->dict.find(find);
         if (it != this->dict.end()) {
            result = std::regex_replace(match[0].str(), r, it->second);
         }

         for (int i = 1; i < match.size(); i++) {
            auto it = this->dict.find(match[i]);
            if (it != this->dict.end()) {
               result.replace(result.find(match[i]), match[i].length(), it->second);
            }
         }
         this->dict_log.emplace(std::pair<std::string, std::string>(key, result));
         return result;
      }
   }
   return std::nullopt;
}

std::optional<std::string> Dictionary::Get(const std::string& key)
{
   if (key.empty()) {
      return std::nullopt;
   }

   if (this->dict_log.find(key) != this->dict_log.end()) {
      return this->dict_log.at(key);
   }

   spdlog::debug("regex search");
   auto ret = RegexSearch(key);
   if (ret) return ret;

   if (this->dict.find(key) == this->dict.end()) {
      return std::nullopt;
   }

   auto value = this->dict.at(key);
   if (value.empty()) {
      this->dict_log.emplace(std::pair<std::string, std::string>(key, ""));
      return std::nullopt;
   }
   this->dict_log.emplace(std::pair<std::string, std::string>(key, value));
   return value;
}

void Dictionary::PrepareBufferOut()
{
   int index = 0;
   std::string word;
   while (this->trans_buffer >> word) {
      index += word.length() + 1;
      if (index > this->line_limit) {
         this->out_buffer << "\n";
         index = 0;
      }
      this->out_buffer << word + " ";
   }

   index = 0;
   for (std::string line; std::getline(this->out_buffer, line);) {
      std::string key = this->out_vector[index].first;
      int y = this->out_vector[index].second;
      if (index + start_xy.second == y) {
         this->dict_log.emplace(std::pair<std::string, std::string>(key, line));
         spdlog::debug("bufferlog input {} {}");
      } else {
         this->dict_log.emplace(std::pair<std::string, std::string>(key, key));
         spdlog::debug("bufferlog can't find {} {}");
      }
      index++;
   }

   // 초기화
   this->str_buffer.str("");
   this->trans_buffer.str("");
   this->out_xy = this->start_xy;
   this->start_xy.first = -1;
   this->start_xy.second = -1;
   this->out_buffer.str("");
   this->out_vector.clear();

   this->buffer_ready = false;
}

void Dictionary::SetStringCoord(int x, int y)
{
   if (this->start_xy.first == -1) this->start_xy.first = x;
   if (this->start_xy.second == -1) this->start_xy.second = this->now_y = y;
   if (this->now_y - y < -1) this->str_buffer << "  \n";
   if (this->now_y < y) this->now_y = y;
}

void Dictionary::MakeupString()
{
   spdlog::debug("#makeup buffer : {}", this->str_buffer.str());

   for (std::string line; std::getline(this->str_buffer, line);) {
      spdlog::debug("##colorst start translate :{} ", line);

      std::string comma;
      if (line.find(',') != std::string::npos) {
         line.erase(line.length() - 1);
         // comma += ",";
      }
      spdlog::debug("##get trans :{} ", line);

      if (!line.empty()) {
         auto res = this->Get(line);
         if (res) {
            res.value() += comma + "\n";
            this->trans_buffer << res.value();
            spdlog::debug("###colorst translate: {} : {}", line, res.value());

         } else
            this->trans_buffer << line + "\n";
      } else
         this->trans_buffer << line + "\n";

      spdlog::debug("colorst end translate## :{}", line);
   }
}

bool Dictionary::StoreStringBuffer(const char* key, int x, int y)
{
   bool is_ready = false;
   if (this->start_xy.second >= y) {
      if (this->buffer_ready == false && this->out_vector[0].first.compare(key) == 0 && this->start_xy.second == y) {
         this->buffer_ready = true;
         spdlog::debug("start makeup # str{} x{} y{}", key, x, y);
         MakeupString();
         spdlog::debug("start prepare out");
         PrepareBufferOut();
         is_ready = true;
      } else {
         spdlog::debug("store return");
         return false;
      }
   }
   SetStringCoord(x, y);

   // 문자열 저장
   bool check_blank = false;
   if (key[0] > 96 && key[0] < 123) this->str_buffer << " ";
   for (int i = 0; i < strlen(key); i++) {
      if (check_blank) {
         check_blank = false;
         if (key[i] == ' ') continue;
      }
      this->str_buffer << key[i];
      if (key[i] == ',' || key[i] == '.') {
         this->str_buffer << "\n";
         check_blank = true;
      }
   }

   this->out_vector.emplace_back(std::pair<std::string, int>(key, y));

   return is_ready;
}

std::optional<std::string> Dictionary::GetMulti(const char* key, int x, int y)
{
   auto len = strnlen_s(key, 1000);
   if (!key || len <= 0 || len >= 1000) {
      return std::nullopt;
   }
   // spdlog::debug("log not find", key);
   if (this->dict_log.find(key) != this->dict_log.end()) {
      return this->dict_log.at(key);
   }

   StoreStringBuffer(key, x, y);

   return std::nullopt;
}

void Dictionary::CompleteBuffer()
{
   this->out_buffer.str("");
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
   this->dict_log.clear();
   this->regex_set.clear();
}