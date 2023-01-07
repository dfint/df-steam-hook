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

uint32_t checksum(const std::string filename)
{
  std::ifstream file(filename);
  uint32_t checksum = 0;
  unsigned shift = 0;
  for (uint32_t ch = file.get(); file; ch = file.get()) {
    checksum += (ch << shift);
    shift += 8;
    if (shift == 32) {
      shift = 0;
    }
  }
  return checksum;
}

#endif