#include "dictionary.h"

namespace Patches {

  inline auto module_handle = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

  void* GetAddress(uintptr_t offset)
  {
    return reinterpret_cast<void*>(module_handle + offset);
  }

  size_t Align(size_t n, size_t edge = 4)
  {
    return (n + edge - 1) & (-edge);
  }

  void PatchBytes(void* address, const char* bytes, size_t size)
  {
    if (size == 0) {
      return;
    }

    if (reinterpret_cast<uintptr_t>(address) == 0xFFFFFFFE) {
      logger::critical("failed while patching bytes at 0x{:x}", reinterpret_cast<uintptr_t>(address));
      MessageBoxA(nullptr, "invalid address to patch", "dfint hook error", MB_ICONERROR);
      exit(2);
    }

    DWORD protection = PAGE_EXECUTE_READWRITE;
    VirtualProtect(address, size, protection, &protection);
    memcpy(address, bytes, size);
    VirtualProtect(address, size, protection, &protection);
  }

  void TranslateStringByOffset(uintptr_t offset)
  {
    auto address = GetAddress(offset);
    auto str = reinterpret_cast<char*>(address);
    auto len = Align(strnlen_s(str, 100) + 1);
    auto tstr = Dictionary::GetSingleton()->Get(str);
    if (tstr) {
      tstr.value().resize(len - 1);
      PatchBytes(address, tstr.value().c_str(), len);
    }
  }

  void BatchStringPatching()
  {
    for (auto& offset : Config::Offset::string_patches) {
      TranslateStringByOffset(offset);
    }
  }

  void Install()
  {
    BatchStringPatching();
    logger::info("patches installed");
  }

} // namespace Patches