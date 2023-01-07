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

      if (Config::Metadata::name != "dfint localization hook") {
        spdlog::critical("unable to find config file");
        exit(2);
      }
      spdlog::info("config version: {}", Config::Metadata::version);

      Dictionary::GetSingleton()->LoadCsv("dfint_dictionary.csv");

      DetourRestoreAfterWith();
      DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());
      InstallTranslation();
      // InstallTTFInjection();
      // InstallStateManager();
      DetourTransactionCommit();
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
}
