#ifndef _UTILS_INCLUDED_
#define _UTILS_INCLUDED_

std::u16string cp437_to_unicode(const std::string& str)
{
  std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> ucs2conv;
  try {
    std::u16string ucs2 = ucs2conv.from_bytes(str);
    return ucs2;
  } catch (const std::range_error& e) {
    spdlog::error("some error while encoding from utf8 to ucs2");
  }
}

std::u16string s2s16(const std::string& str)
{
  std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> converter;
  return converter.from_bytes(str);
}

std::wstring s2ws(const std::string& str)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  return converter.from_bytes(str);
}

std::string ws2s(const std::wstring& wstr)
{
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  return converter.to_bytes(wstr);
}

time_t petimestamp(const std::string filename)
{
  std::ifstream file(filename);

  if (!file.is_open()) {
    spdlog::error("unable to open PE file {}", filename);
  }
  char buf[64];
  file.get(buf, 64);
  auto dos_header = reinterpret_cast<IMAGE_DOS_HEADER*>(buf);
  file.seekg(dos_header->e_lfanew);
  file.get(buf, 64);
  auto nt_header = reinterpret_cast<IMAGE_NT_HEADERS*>(buf);

  return nt_header->FileHeader.TimeDateStamp;
}

#endif