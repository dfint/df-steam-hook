#include <Windows.h>
#include <cstdio>
#include <detours/detours.h>

int main() {
  STARTUPINFO si;
  PROCESS_INFORMATION pi;

  // Initialize si&pi fill with 0
  memset(&si, 0, sizeof(si));
  memset(&pi, 0, sizeof(pi));
  si.cb = sizeof(si);

  // spawn a DLL injected process
  const BOOL status = DetourCreateProcessWithDllEx("Dwarf Fortress.exe",          // application name
                                                   nullptr,                       // full command line + arguments
                                                   nullptr,                       // process attributes
                                                   nullptr,                       // thread attributes
                                                   FALSE,                         // inherit handles
                                                   CREATE_DEFAULT_ERROR_MODE,     // creation flags
                                                   nullptr,                       // environment
                                                   nullptr,                       // current directory
                                                   &si,                           // startup info
                                                   &pi,                           // process info
                                                   "./dfint_data/dfint_hook.dll", // path to dll to inject
                                                   nullptr);                      // use standard CreateProcess API

  if (status) {
    printf("launched DF PID %d!\n", pi.dwProcessId);
  } else {
    printf("failed to launch...\n");
  }

  return 0;
}