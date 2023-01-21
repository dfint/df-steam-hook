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
      ReplaceAll(arg, "\\", "$");
      this->Add(key, arg);

      if (key.find("\\s") != std::string::npos)
      {
         ReplaceAll(key, "\\s", "(.*)");
         this->regex_set.emplace(key);
      }
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
   std::smatch match;
   for (const auto regex_string : this->regex_set)
   {
      std::regex r(regex_string);
      // spdlog::debug("#regex string {}", regex_string);
      if (std::regex_match(key, match, r) && match.size() > 1)
      {
         // spdlog::debug("#FIND {}", key);

         std::string find(regex_string);
         ReplaceAll(find, "(.*)", "\\s");

         std::string result;
         auto it = this->dict.find(find);
         if (it != this->dict.end())
         {
            result = std::regex_replace(match[0].str(), r, it->second);
         }

         for (int i = 1; i < match.size(); i++)
         {
            auto it = this->dict.find(match[i]);
            if (it != this->dict.end())
            {
               result.replace(result.find(match[i]), match[i].length(), it->second);
            }
         }
         this->dict_log.emplace(std::pair<std::string, std::string>(key, result));
         return result;
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

   if (this->dict.find(key) == this->dict.end())
   {
      return std::nullopt;
   }

   auto value = this->dict.at(key);
   if (value.empty())
   {
      // this->dict_log.emplace(std::pair<std::string, std::string>(key, ""));
      return std::nullopt;
   }
   this->dict_log.emplace(std::pair<std::string, std::string>(key, value));
   return value;
}

void Dictionary::PrepareBufferOut()
{
   // std::stringstream temp;
   int index = 0;
   std::string temp;
   //std::vector<std::string> key_vector(this->key_vec.begin(), this->key_vec.end());
   while (index < this->key_vec.size())
   {
      std::string key_line;
      if (this->value_queue.empty())
      {
         key_line = this->key_vec[index];
         std::string last_text = "@";
         if(!temp.empty())
         {
            last_text = temp;
            temp = "";
         }
         this->dict_multi.emplace(key_line, last_text);
         spdlog::debug("## DICT EMPTY VALUE {}#{}", key_line, temp);
         index++;
      }
      else
      {
         std::stringstream value_line(this->value_queue.front());
         this->value_queue.pop();
         // spdlog::debug("## VALUE {}", value_line.str());
         std::string word;
         while (value_line >> word)
         {
            std::wstring wstr = Utils::s2ws(temp+word);
            if (wstr.length() + 1 > this->line_limit)
            {
               key_line = this->key_vec[index];
               spdlog::debug("## INPUT:{} key|{}| value|{}|", index, key_line, temp);
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

   this->start_y = -1;
   this->now_y = -1;
   this->is_ready = false;
   spdlog::debug("## Clear");
}

void Dictionary::MakeupString()
{
   std::string temp;
   int endl = 0;
   for (auto &line : this->key_vec)
   {
      spdlog::debug("##value in::{}", line);
      std::stringstream key_line(line);
      std::string word;
      while (key_line >> word)
      {
         if (word.find(',') != std::string::npos || word.find('.') != std::string::npos)
         {
            std::string comma = word.substr(word.length() - 1);
            word.erase(word.length() - 1);
            temp += word;
            spdlog::debug("###TRANS IN:{}", temp);
            auto ret = Get(temp);
            if (ret)
            {
               temp = ret.value() + comma;
               this->value_queue.push(temp);
               spdlog::debug("#Value IN:{}", temp);
            }
            else
            {
               temp += comma;
               this->value_queue.push(temp);
               spdlog::debug("#No trans IN:{}", temp);
            }
            temp = "";
         }
         else
            temp += word + " ";
      }
   }
}

std::optional<std::string> Dictionary::StoreStringBuffer(const char *key, int x, int y)
{
   if (this->start_y == y)
   {
      this->is_ready = true;
      MakeupString();
      PrepareBufferOut();
   }

   if (this->is_ready) return std::nullopt;
   if (this->dict_multi.find(key) != this->dict_multi.end())
   {
      return this->dict_multi.at(key);
      spdlog::debug("### log FIND key::{}::find::{}", key, this->dict_multi.at(key));
   }
   // spdlog::debug("## IN {}", y);
   if (this->start_y == -1)
   {
      this->start_y = y;
      this->now_y = y;
      spdlog::debug("START line input {} key {}", this->now_y, key);
      //auto ret = std::find(this->key_vec.begin(), this->key_vec.end(), key);
      this->key_vec.emplace_back(key);
      spdlog::debug("## INPUT key {}", key);
   }

   if (this->now_y<y &&this->now_y> 0)
   {
      this->now_y = y;
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
   this->regex_set.clear();
}