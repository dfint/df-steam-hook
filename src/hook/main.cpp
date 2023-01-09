#include "dictionary.h"
#include "hooks.h"
#include "logger.hpp"
#include "utils.hpp"

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
        MessageBoxA(nullptr, "unable to find config file", "dfint hook error", MB_ICONERROR);
        exit(2);
      }
      if (Config::Metadata::checksum != petimestamp("Dwarf Fortress.exe")) {
        spdlog::critical("checksum mismatch, seems like config for another version of DF");
      } else {
        spdlog::info("checksum ok!");
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
