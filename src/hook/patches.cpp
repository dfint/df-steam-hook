#include "dictionary.h"

namespace Patches {
  inline auto module_handle = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

  void* GetAddress(uintptr_t offset)
  {
    return reinterpret_cast<void*>(module_handle + offset);
  }

  inline void ChangeProtection(void* ptr, size_t size, DWORD& protection)
  {
    VirtualProtect(ptr, size, protection, &protection);
  }

  size_t Align(size_t n, size_t edge = 8)
  {
    return (n + edge - 1) & (-edge);
  }

  void PatchBytes(void* address, const char* bytes, size_t size)
  {
    if (size == 0) {
      return;
    }

    if (reinterpret_cast<uintptr_t>(address) == 0xFFFFFFFE) {
      MessageBoxA(nullptr, "invalid address to patch", "dfint hook error", MB_ICONERROR);
      exit(2);
    }

    DWORD protection = PAGE_EXECUTE_READWRITE;
    ChangeProtection(address, size, protection);
    memcpy(address, bytes, size);
    ChangeProtection(address, size, protection);
  }

  void TranslateStringByOffset(uintptr_t offset)
  {
    auto address = GetAddress(offset);
    auto str = reinterpret_cast<char*>(address);
    auto len = Align(strnlen_s(str, 100) + 1);
    auto tstr = Dictionary::GetSingleton()->Get(str);
    if (tstr) {
      tstr.value().resize(len);
      PatchBytes(address, tstr.value().c_str(), len);
    }
  }

  void Install()
  {
    TranslateStringByOffset(0x10C1D40);
    TranslateStringByOffset(0x10C1D90);
    TranslateStringByOffset(0x10C1D98);

    logger::info("patches installed");
  }

} // namespace Patches