#include "dictionary.h"

namespace Watchdog {
  namespace Handler {

    enum Action {
      TRANSLATE_OFF,
      TRANSLATE_ON,
      DICTIONARY_RELOAD
    };

    std::experimental::generator<Action> WaitForAction() {
      while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if ((GetAsyncKeyState(VK_CONTROL) & GetAsyncKeyState(VK_F3)) && Config::Setting::enable_translation == true) {
          co_yield Action::TRANSLATE_OFF;
        } else if ((GetAsyncKeyState(VK_CONTROL) & GetAsyncKeyState(VK_F4)) && Config::Setting::enable_translation == false) {
          co_yield Action::TRANSLATE_ON;
        } else if (GetAsyncKeyState(VK_CONTROL) & GetAsyncKeyState(VK_F2)) {
          co_yield Action::DICTIONARY_RELOAD;
        }
      }
    }

    void Keypress() {
      for (auto& action : WaitForAction()) {
        switch (action) {
          case TRANSLATE_OFF: {
            Config::Setting::enable_translation = false;
            logger::info("translation switched off");
            MessageBoxA(nullptr, "translation switched off", "dfint hook info", MB_ICONWARNING);
            break;
          }
          case TRANSLATE_ON: {
            Config::Setting::enable_translation = true;
            logger::info("translation switched on");
            MessageBoxA(nullptr, "translation switched on", "dfint hook info", MB_ICONWARNING);
            break;
          }
          case DICTIONARY_RELOAD: {
            logger::info("reload dictionary");
            Dictionary::GetSingleton()->Clear();
            Dictionary::GetSingleton()->LoadCsv(Config::Setting::dictionary);
            MessageBoxA(nullptr, "dictionary reloaded", "dfint hook info", MB_ICONWARNING);
            break;
          }
        }
      }
    }

  } // namespace Handler

  void WatchKeyboard() {
    std::thread{ std::ref(Watchdog::Handler::Keypress) }.detach();
  }
} // namespace Watchdog