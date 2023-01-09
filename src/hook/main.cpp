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
        logger::critical("unable to find config file");
        MessageBoxA(nullptr, "unable to find config file", "dfint hook error", MB_ICONERROR);
        exit(2);
      }
      logger::info("pe checksum: 0x{:x}", Config::Metadata::checksum);
      logger::info("offsets version: {}", Config::Metadata::version);

      Dictionary::GetSingleton()->LoadCsv("./dfint_data/dfint_dictionary.csv");

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
