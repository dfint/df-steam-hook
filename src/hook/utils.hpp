
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

// std::wstring cp1251ucs2(const std::string& str)
// {
//   std::wstring_convert<codecvt_cp1251<char, wchar_t, mbstate_t>> converter;
//   return converter.from_bytes(str);
// }