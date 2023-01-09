#ifndef _UTILS_INCLUDED_
#define _UTILS_INCLUDED_

namespace Utils {
  inline std::u16string cp437_to_unicode(const std::string& str)
  {
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> ucs2conv;
    try {
      std::u16string ucs2 = ucs2conv.from_bytes(str);
      return ucs2;
    } catch (const std::range_error& e) {
      spdlog::error("some error while encoding from utf8 to ucs2");
    }
  }

  inline std::u16string s2s16(const std::string& str)
  {
    std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> converter;
    return converter.from_bytes(str);
  }

  inline std::wstring s2ws(const std::string& str)
  {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(str);
  }

  inline std::string ws2s(const std::wstring& wstr)
  {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.to_bytes(wstr);
  }

}
#endif