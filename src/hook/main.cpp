#include "crash_report.hpp"
#include "dictionary.h"
#include "hooks.h"
#include "logger.hpp"
#include "patches.h"
#include "watchdog.hpp"

extern "C" __declspec(dllexport) VOID NullExport(VOID) {}

BOOL ProcessAttach() {
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
  if (Config::Setting::enable_search) {
    Hooks::InstallTextEntry();
  }
  // Hooks::InstallTTFInjection();
  // Hooks::InstallStateManager();

  if (Config::Setting::enable_patches) {
    Patches::Install();
  }
  DetourTransactionCommit();
  logger::info("hooks installed");

  return TRUE;
}

BOOL ProcessDetach() {
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());

  if (Config::Setting::enable_translation) {
    Hooks::UninstallTranslation();
  }
  if (Config::Setting::enable_search) {
    Hooks::UninstallTextEntry();
  }
  // Hooks::UninstallTTFInjection();
  // Hooks::UninstallStateManager();
  logger::info("hooks uninstalled");
  DetourTransactionCommit();

  return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
  if (Config::Setting::crash_report) {
    CrashReport::Install();
  }

  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
      return ProcessAttach();
    case DLL_PROCESS_DETACH:
      return ProcessDetach();
    default:
      return TRUE;
  }
}
