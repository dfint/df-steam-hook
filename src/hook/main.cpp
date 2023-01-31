#include "crash_report.hpp"
#include "dictionary.h"
#include "hooks.h"
#include "logger.hpp"
#include "patches.h"
#include "watchdog.hpp"

extern "C" __declspec(dllexport) VOID NullExport(VOID)
{
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
  if (Config::Setting::crash_report) {
    CrashReport::Install();
  }

  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
      InitLogger();
      if (Config::Setting::watchdog) {
        Watchdog::WatchKeyboard();
      }

      if (Config::Metadata::name != "dfint localization hook") {
        logger::critical("unable to find config file");
        MessageBoxA(nullptr, "unable to find config file", "dfint hook error", MB_ICONERROR);
        exit(2);
      }
      logger::info("hook version: {}", Config::Metadata::hook_version);
      logger::info("pe checksum: 0x{:x}", Config::Metadata::checksum);
      logger::info("offsets version: {}", Config::Metadata::version);

      Dictionary::GetSingleton()->LoadCsv(Config::Setting::dictionary);

      DetourRestoreAfterWith();
      DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());

      if (Config::Setting::enable_translation) {
        Hooks::InstallTranslation();
      }
      // Hooks::InstallTTFInjection();
      // Hooks::InstallStateManager();

      // Patches::Install();

      DetourTransactionCommit();
      logger::info("hooks installed");
      break;
    }
    case DLL_PROCESS_DETACH: {
      DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());

      if (Config::Setting::enable_translation) {
        Hooks::UninstallTranslation();
      }
      // Hooks::UninstallTTFInjection();
      // Hooks::UninstallStateManager();
      logger::info("hooks uninstalled");

      DetourTransactionCommit();
      break;
    }
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
  }
  return TRUE;
}
