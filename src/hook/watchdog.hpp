namespace Watchdog {

  namespace Handler {

    void Keypress()
    {
      while (true) {
        Sleep(10);
        if ((GetAsyncKeyState(VK_CONTROL) & GetAsyncKeyState(VK_F3)) && Config::Setting::enable_translation == true) {
          Config::Setting::enable_translation = false;
          logger::info("translation switched off");
          MessageBoxA(nullptr, "translation switched off", "dfint hook info", MB_ICONWARNING);
        }
        if ((GetAsyncKeyState(VK_CONTROL) & GetAsyncKeyState(VK_F4)) && Config::Setting::enable_translation == false) {
          Config::Setting::enable_translation = true;
          logger::info("translation switched on");
          MessageBoxA(nullptr, "translation switched on", "dfint hook info", MB_ICONWARNING);
        }
      }
    }

  }

  void WatchKeyboard()
  {
    CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Watchdog::Handler::Keypress), nullptr, 0,
                 nullptr);
  }

}