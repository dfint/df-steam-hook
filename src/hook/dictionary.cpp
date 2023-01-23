#include "dictionary.h"

#include "utils.hpp"

void Dictionary::ReplaceAll(std::string &subject, const std::string &search, const std::string &replace)
{
   size_t pos = 0;
   while ((pos = subject.find(search, pos)) != std::string::npos)
   {
      subject.replace(pos, search.length(), replace);
      pos += replace.length();
   }
}

std::string Dictionary::Sanitize(std::string &str)
{
   ReplaceAll(str, R"(\\)", "\\");
   ReplaceAll(str, R"(\t)", "\t");
   ReplaceAll(str, R"(\r)", "\r");
   ReplaceAll(str, R"(\n)", "\n");
   ReplaceAll(str, R"(")", "\"");
   return str;
}

void Dictionary::SplitRegex(const std::string &str)
{
   const std::regex re("(.*)=(.*)");

   std::smatch match;
   if (std::regex_search(str, match, re))
   {
      std::string key = match[1];
      std::string arg = match[2];
      key = Sanitize(key);
      arg = Sanitize(arg);
      this->Add(key, arg);

      RegexRplace(key, true);
      this->regex_vector.emplace_back(key);
   }
}

void Dictionary::RegexRplace(std::string &str, bool on)
{
   if (on)
   {
      ReplaceAll(str, "{s}", "([A-Za-z\\s]*)[.|,]");
      ReplaceAll(str, "{s,}", "([A-Za-z\\s].*)");
      ReplaceAll(str, "{d}", "([\\d]*)");
   }
   else
   {
      ReplaceAll(str, "([A-Za-z\\s]*)[.|,]", "{s}");
      ReplaceAll(str, "([A-Za-z\\s].*)", "{s,}");
      ReplaceAll(str, "([\\d]*)", "{d}");
   }
}

std::pair<std::string, std::string> Dictionary::Split(const std::string &str)
{
   std::string delimiter = "\",\"";
   auto delimiter_pos = str.find(delimiter);
   auto key = str.substr(1, delimiter_pos - 1);
   // value length = str-key-1(for fisrt ")-1(for last ")-3(for delimiter)
   auto value = str.substr(delimiter_pos + 3, str.length() - key.length() - 5);
   return std::make_pair(Sanitize(key), Sanitize(value));
}

// C code would be faster here, but we need to load just once
void Dictionary::LoadCsv(const std::string &filename, const std::string &regex_filename)
{
   logger::info("trying to load dictionary from csv file {}", filename);
   std::ifstream file(filename);
   if (!file.is_open())
   {
      logger::critical("unable to open csv file {}", filename);
      MessageBoxA(nullptr, "unable to find csv dictionary file", "dfint hook error", MB_ICONERROR);
      exit(2);
      // do we need exit(2) here?
      return;
   }

   std::string line;
   int i = 0;
   while (std::getline(file, line))
   {
      auto pair = Split(line);
      this->Add(pair);
   }
   file.close();
   logger::info("csv dictionary loaded, total lines {}", this->Size());

   std::ifstream regex_file;
   regex_file.open(regex_filename);
   if (!regex_file.is_open())
   {
      logger::critical("unable to open regex file {}", regex_filename);
      MessageBoxA(nullptr, "unable to open regex file", "dfint hook error", MB_ICONERROR);
      exit(2);
      // do we need exit(2) here?
      return;
   }

   while (std::getline(regex_file, line))
   {
      SplitRegex(line);
   }
   regex_file.close();
   logger::info("add regex in csv dictionary, total lines {}", this->Size());
}

std::optional<std::string> Dictionary::RegexSearch(const std::string &key)
{
   // spdlog::debug("\n#Input key {}", key);

   std::smatch match;
   for (const auto regex_string : this->regex_vector)
   {
      std::regex r(regex_string);
      // spdlog::debug("#regex {}", regex_string);
      if (std::regex_match(key, match, r) && match.size() > 1)
      {
         std::string find(regex_string);
         RegexRplace(find, false);
         std::string result;

         auto it = this->dict.find(find);
         if (it != this->dict.end())
         {
            result = std::regex_replace(match[0].str(), r, it->second);
            spdlog::debug("\n#FIND {}", key);
            spdlog::debug("#REGEX {}", find);
            spdlog::debug("#Match0 {}", result);

            for (int i = 1; i < match.size(); i++)
            {
               std::string matched(match[i].str());
               spdlog::debug("#Match{} {}", i, matched);
               auto it = Get(matched);
               // if (it == this->dict.end() && matched.find(".") != std::string::npos)
               // {
               //    std::string temp = matched.substr(0, matched.find("."));
               //    it = this->dict.find(temp);
               //    spdlog::debug("#Match{} {}", i, temp);
               // }
               spdlog::debug("#Match{} {}", i, matched);

               // if (matched.compare(".") == 0 || matched.compare(",") == 0)
               // {
               //    result += matched;
               //    return result;
               // }
               // spdlog::debug("#REPLACE {} s{} t{} {}", matched, matched.length(), it.value().length(), match.size());
               if (it)
               {
                  spdlog::debug("#replace???? {}", result);
                  result.replace(result.find(matched), matched.length(), it.value());
               }

               spdlog::debug("#Result {}", result);
            }
            this->dict_log.emplace(std::pair<std::string, std::string>(key, result));
            spdlog::debug("#dict log input {}\n", result);
            return result;
         }
         else
         {
            return std::nullopt;
         }
      }
   }
   return std::nullopt;
}

std::optional<std::string> Dictionary::Get(const std::string &key)
{
   if (key.empty())
   {
      return std::nullopt;
   }

   if (this->dict_log.find(key) != this->dict_log.end())
   {
      return this->dict_log.at(key);
   }

   if (this->dict.find(key) != this->dict.end())
   {
      return this->dict.at(key);
   }
   // spdlog::debug("regex search");
   auto ret = RegexSearch(key);
   if (ret) return ret;

   return std::nullopt;
}

void Dictionary::PrepareBufferOut()
{
   int index = 0;
   std::string temp;
   while (index < this->key_vec.size())
   {
      std::string key_line;
      if (this->value_queue.empty())
      {
         key_line = this->key_vec[index];
         std::string last_text = "-";
         if (!temp.empty())
         {
            last_text = temp;
            temp = "";
         }
         this->dict_multi.emplace(key_line, last_text);
         spdlog::debug("##DICT EMPTY VALUE {} # {}", key_line, last_text);
         index++;
      }
      else
      {
         std::stringstream value_line(this->value_queue.front());
         this->value_queue.pop();
         std::string word;
         while (value_line >> word)
         {
            std::wstring wstr = Utils::s2ws(temp + word);
            if (wstr.length() + 1 > this->line_limit)
            {
               key_line = this->key_vec[index];
               spdlog::debug("##DICT INPUT:{} key:{} value:{}", index, key_line, temp);
               this->dict_multi.emplace(key_line, temp);
               temp = "";
               index++;
            }
            temp += word + " ";
         }
      }
   }

   InitBufferVar();
}

void Dictionary::InitBufferVar()
{
   if (!this->value_queue.empty())
   {
      spdlog::debug("text remain {}", this->value_queue.front());
      for (int i = 0; i < this->value_queue.size(); i++)
      {
         this->value_queue.pop();
      }
   }
   this->key_vec.clear();

   this->start_line = -1;
   this->pre_line = -1;
   this->is_continuous = false;
   spdlog::debug("## Clear");
}

void Dictionary::MakeupString()
{
   std::string temp;
   int endl = 0;
   for (auto &line : this->key_vec)
   {
      spdlog::debug("\n#2#Line SRC in::{}", line);
      temp += line;
      spdlog::debug("\n#2#Line + in::{}", temp);

      if (line.find(".") != std::string::npos || line.find(",") != std::string::npos)
      {
         std::stringstream key_line(temp);
         temp = "";
         std::string word;
         while (key_line >> word)
         { // ',' '.' 기준으로 입력받은 문자열 줄을 분리

            spdlog::debug("#2#while :{}", word);
            spdlog::debug("#2#temp :{}", temp);

            if (word.find(',') != std::string::npos || word.find('.') != std::string::npos)
            {
               // std::string comma = word.substr(word.length() - 1);
               // word.erase(word.length() - 1);
               temp += word;
               spdlog::debug("\n#2#SPLIT :{}", line);
               auto ret = Get(temp);
               if (ret)
               {
                  temp = ret.value(); // + comma;
                  this->value_queue.push(temp);
                  spdlog::debug("#2#Value IN:{}", temp);
               }

               else
               {
                  // temp += comma;
                  this->value_queue.push(temp);
                  spdlog::debug("#2#NoTrans IN:{}", temp);
               }
               temp = "";
            }
            else
               temp += word + " ";
         }
      }
      else
      {
         spdlog::debug("\n#2#?????? in::{}", temp);
         auto ret = Get(temp);
         if (ret)
         {
            spdlog::debug("\n#2#??dict???? in::{}", ret.value());
            this->value_queue.push(ret.value());
         }
      }
   }
}

std::optional<std::string> Dictionary::StoreStringBuffer(const char *key, int x, int y)
{
   std::string temp(key);

   if (this->start_line != -1 && (y - this->pre_line > 1 || this->start_line == y))
   {
      MakeupString();
      PrepareBufferOut();
   }

   if (this->start_line != -1 && this->is_continuous)
   {
      this->pre_line = y;
      this->key_vec.emplace_back(key);
      spdlog::debug("## INPUT key {}", key);
      return std::nullopt;
   }

   if (this->dict_multi.find(key) != this->dict_multi.end())
   {
      return this->dict_multi.at(key);
   }

   // 문자열 확인
   if (this->start_line == -1)
   {
      this->start_line = y;
      this->pre_line = y;
      this->is_continuous = true;
      spdlog::debug("START line input {}", this->pre_line);
      this->key_vec.emplace_back(key);
      spdlog::debug("## INPUT key {}", key);
   }

   return std::nullopt;
}

std::optional<std::string> Dictionary::GetMulti(const char *key, int x, int y)
{
   auto len = strnlen_s(key, 1000);
   if (!key || len <= 1 || len >= 1000)
   {
      return std::nullopt;
   }

   auto ret = StoreStringBuffer(key, x, y);
   if (ret) return ret.value();

   return std::nullopt;
}

void Dictionary::CompleteBuffer()
{
}

bool Dictionary::Exist(std::string &key)
{
   return this->dict.find(key) != this->dict.end();
}

void Dictionary::Add(std::string &key, std::string &value)
{
   this->dict.emplace(std::make_pair(key, value));
}

void Dictionary::Add(std::pair<std::string, std::string> &pair)
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
   this->dict_multi.clear();
   this->regex_vector.clear();

   void InitBufferVar();
}