#include "dictionary.h"
#include "hooks.h"
#include "logger.hpp"

extern "C" __declspec(dllexport) VOID NullExport(VOID)
{
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
      InitLogger();
      Dictionary::GetSingleton()->LoadCsv("df_translation.csv");

      DetourRestoreAfterWith();
      DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());
      InstallHooks();
      DetourTransactionCommit();
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
}
