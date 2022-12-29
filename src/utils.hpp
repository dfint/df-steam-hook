#include "hooks.h"

void CreateDFString(DFString_& target, const std::string& str)
{
  auto len = str.size();
  target.len = len;

  if (len > 15) {
    std::vector<char> cstr(str.c_str(), str.c_str() + str.size() + 1);
    target.ptr = cstr.data();
    target.capa = len;
  } else {
    target.pad = 0;
    target.capa = 15;
    strcpy(target.buf, str.c_str());
  }
}

// std::u16string utf_to_unicode(const std::string& str)
// {
//   std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> ucs2conv;
//   try {
//     std::u16string ucs2 = ucs2conv.from_bytes(str);
//     return ucs2;
//   } catch (const std::range_error& e) {
//     spdlog::error("some error while encoding from utf8 to ucs2");
//   }
// }