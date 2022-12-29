#include "hooks.h"

void CreateDFString(DFString_* target, const std::string& str)
{
  auto len = str.size();
  target->len = len;

  if (len > 15) {
    std::vector<char> cstr(str.c_str(), str.c_str() + str.size() + 1);
    target->ptr = cstr.data();
    target->capa = len;
  } else {
    target->pad = 0;
    target->capa = 15;
    strcpy(target->buf, str.c_str());
  }
}