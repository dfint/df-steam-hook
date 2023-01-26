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

std::string Dictionary::EraseFrontBackBlank(std::string &str)
{
   int index;
   index = str.find_first_not_of(" ");
   str.erase(0, index);
   index = str.find_last_not_of(" ");
   str.erase(index + 1);
   return str;
}

std::string Dictionary::EraseStringComma(std::string &str)
{
   ReplaceAll(str, ",", "");
   ReplaceAll(str, ".", "");
   ReplaceAll(str, "  ", " ");
   return str;
}

std::string Dictionary::Sanitize(std::string &str)
{
   ReplaceAll(str, R"(\\)", "\\");
   ReplaceAll(str, R"(\t)", "\t");
   ReplaceAll(str, R"(\r)", "\r");
   ReplaceAll(str, R"(\n)", "\n");
   ReplaceAll(str, R"(")", "\"");

   std::regex r("\\[[^\\]]+\\]");
   str = std::regex_replace(str, r, "");
   

   // spdlog::debug("Dictionary :{}", str);
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
      this->Add(key, arg);

      RegexRplace(key, true);
      this->regex_vector.emplace_back(key);
   }
}

void Dictionary::RegexRplace(std::string &str, bool on)
{
   if (on)
   {
      ReplaceAll(str, "{s}", "([^\\.^\\,]*)[,.]");
      ReplaceAll(str, "{s,}", "(.*)");
      ReplaceAll(str, "{d}", "([\\d]*)");
   }
   else
   {
      ReplaceAll(str, "([^\\.^\\,]*)[,.]", "{s}");
      ReplaceAll(str, "(.*)", "{s,}");
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
   
   Sanitize(key);
   EraseStringComma(key);
   return std::make_pair(EraseFrontBackBlank(key), Sanitize(value));
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
   std::string input(key);
   std::smatch match;
   for (const auto regex_string : this->regex_vector)
   {
      std::regex r(regex_string);
      if (std::regex_match(input, match, r) && match.size() > 1)
      {
         std::string find(regex_string);
         RegexRplace(find, false);
         std::string result;

         auto it = this->dict.find(find);
         if (it != this->dict.end())
         {
            result = std::regex_replace(match[0].str(), r, it->second);
            spdlog::debug("\n#Search :{}", input);
            spdlog::debug("#REGEX :{}", find);
            spdlog::debug("#Match0 :{}", result);

            std::string matched;
            for (int i = 1; i < match.size(); i++)
            {
               matched = match[i].str();
               spdlog::debug("#Match{} :{}", i, matched);
               auto it = Get(matched);
               if (it)
               {
                  spdlog::debug("#Replace :{}", result);
                  if (result.find(matched) != std::string::npos) result.replace(result.find(matched), matched.length(), it.value());
                  else spdlog::debug("#Can't find :{}:{}:", matched, result);
               }
               // spdlog::debug("##Result {}", result);
            }
            this->dict_log.emplace(input, result);
            spdlog::debug("##Result :{}\n", result);
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

   std::string input(key);
   EraseFrontBackBlank(input);

   if (this->dict_log.find(input) != this->dict_log.end())
   {
      return this->dict_log.at(input);
   }

   if (this->dict.find(input) != this->dict.end())
   {
      return this->dict.at(input);
   }
   auto ret = RegexSearch(input);
   if (ret) return ret;

   return std::nullopt;
}

void Dictionary::PrepareBufferOut()
{
   int index = 0;
   std::string outLine;
   while (index < this->key_vec.size())
   {
      std::string keyLine = this->key_vec[index];
      if (keyLine.find(this->SKIP) != std::string::npos)
      {
         keyLine.erase(keyLine.find(this->SKIP));
         this->dict_multi.emplace(keyLine, this->SKIP);
         //spdlog::debug("dictIn key {} ",keyLine);
         index++;
      }
      else if (this->value_queue.empty())
      {
         std::string lastText = this->SKIP;
         if (!outLine.empty())
         {
            lastText = outLine;
            outLine = "";
         }
         this->dict_multi.emplace(keyLine, lastText);
         index++;
      }
      else
      {
         std::stringstream valueLine(this->value_queue.front());
         this->value_queue.pop();
         std::string word;
         while (valueLine >> word)
         {
            std::wstring wstr = Utils::s2ws(outLine + word);
            if (wstr.length() + 1 > this->line_limit)
            {
               this->dict_multi.emplace(keyLine, outLine);
               outLine = "";
               index++;
            }
            outLine += word + " ";
         }
      }
   }
}

void Dictionary::InitBuffer()
{
   if (!this->value_queue.empty())
   {
      for (int i = 0; i < this->value_queue.size(); i++)
      {
         this->value_queue.pop();
      }
   }
   this->key_vec.clear();
   this->string_buffer = "";
   this->start_line = -1;
}

void Dictionary::TranslationBuffer()
{
   std::string delimiter = ",.";
   if (this->is_top) delimiter = ".";
   std::string buffer = this->string_buffer;
   spdlog::debug("#Trans SRC:{}", buffer);
   int delimiterPos = 0;
   while (delimiterPos != std::string::npos)
   {
      delimiterPos = buffer.find_first_of(delimiter);
      if (delimiterPos != std::string::npos)
      {
         std::string tstr = buffer.substr(0, delimiterPos);
         auto translation = Get(EraseStringComma(tstr));
         if (translation)
         {
            this->value_queue.push(translation.value());
            spdlog::debug("#Trans RESULT:{}", translation.value());
         }
         else
         {
            this->value_queue.push(tstr);
            spdlog::debug("Trans FAIL:{}", tstr);
         }
         buffer.erase(0, delimiterPos + 2);
      }
   }

   if (buffer.size() > 0)
   {
      spdlog::debug("Tran LEFT:{}", buffer);

      // auto translation = Get(buffer);
      // if (translation) this->value_queue.push(translation.value());
      // else this->value_queue.push(buffer);
   }
}

void Dictionary::FlushBuffer()
{
   //  버퍼가 한줄 일때 바로 번역 처리
   if (this->key_vec.size() == 1)
   {
      auto ret = Get(EraseStringComma(this->string_buffer));
      if (ret)
      {
         this->dict_multi.emplace(this->key_vec.front(), ret.value());
      }
      else
      {
         this->dict_multi.emplace(this->key_vec.front(), this->string_buffer);
      }
      InitBuffer();
   }
   else
   {
      TranslationBuffer();
      PrepareBufferOut();
      InitBuffer();
   }
}

std::optional<std::string> Dictionary::StoreStringBuffer(const char *key, int x, int y)
{
   std::string str(key);
   std::string multiKey = str + "#" + std::to_string(x) + "#" + std::to_string(y);
   // spdlog::debug("Input>{}", str);
   //  버퍼 번역 처리, 같은 줄이 아닐때 이전 줄 번호가 1줄 초과 될때 첫 줄번호가 현재 줄보다 클때
   if (this->start_line != -1 && y != this->pre_line && (y - this->pre_line > 1 || y <= this->start_line))
   {
      FlushBuffer();
   }
   // 이전 결과 처리
   if (this->dict_multi.find(multiKey) != this->dict_multi.end())
   {
      return this->dict_multi.at(multiKey);
   }
   // 첫줄 입력
   if (this->start_line == -1)
   {
      this->start_line = y;
      this->pre_line = y;
      this->string_buffer = str;
      this->key_vec.emplace_back(multiKey);
      // spdlog::debug("##Start buffer {}", this->string_buffer);

      return std::nullopt;
   }
   // 입력
   if (this->start_line != -1)
   {
      if (y == this->pre_line || y == this->start_line)
      {
         std::string skip = multiKey + this->SKIP;
         this->key_vec.emplace_back(skip);
         //spdlog::debug("Skip {}", str);
      }
      else
      {
         this->key_vec.emplace_back(multiKey);
      }
      this->pre_line = y;
      this->string_buffer += " " + str;
      if (this->string_buffer.size() > 1000) InitBuffer();
      // spdlog::debug("Buffer {}", this->string_buffer);
   }
   return std::nullopt;
}

std::optional<std::string> Dictionary::GetMulti(const char *key, int x, int y, bool top)
{
   this->is_top = top;
   auto len = strnlen_s(key, 1000);
   if (!key || len <= 0 || len >= 1000)
   {
      return std::nullopt;
   }

   auto ret = StoreStringBuffer(key, x, y);
   if (ret) return ret.value();
   return std::nullopt;
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

   void InitBuffer();
}