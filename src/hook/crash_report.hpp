#include <Psapi.h>

namespace CrashReport {

  std::ofstream GetCrashReportLogHandle(std::string filename)
  {
    if (!std::filesystem::is_directory("./dfint_data/crash_reports/") ||
        !std::filesystem::exists("./dfint_data/crash_reports/")) {
      std::filesystem::create_directory("./dfint_data/crash_reports/");
    }

    std::ofstream file(filename);
    return file;
  }

  class LoggedStackWalker : public StackWalker
  {
  public:
    LoggedStackWalker()
      : StackWalker()
    {
      this->output_filename = "./dfint_data/crash_reports/cr_" + std::to_string(std::time(NULL)) + ".txt";
      this->output_file = GetCrashReportLogHandle(this->output_filename);
    }

    std::string output_filename;
    std::ofstream output_file;

  protected:
    virtual void OnOutput(LPCSTR szText)
    {
      this->output_file << szText;
      StackWalker::OnOutput(szText);
    }
  };

  uintptr_t GetBaseAddress()
  {

    HWND WindowHandle = FindWindow(nullptr, "Dwarf Fortress");
    DWORD PID;
    GetWindowThreadProcessId(WindowHandle, &PID);
    logger::error("pid {}", PID);
    PVOID hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, 0, PID);

    if (hProcess == NULL)
      return NULL;

    HMODULE lphModule[1024];
    DWORD lpcbNeeded(NULL);

    if (!EnumProcessModules(hProcess, lphModule, sizeof(lphModule), &lpcbNeeded))
      return NULL;

    TCHAR szModName[MAX_PATH];
    if (!GetModuleFileNameEx(hProcess, lphModule[0], szModName, sizeof(szModName) / sizeof(TCHAR)))
      return NULL;

    logger::error("name {}", szModName);

    return (uintptr_t)lphModule[0];
  }

  LONG WINAPI Handler(EXCEPTION_POINTERS* ExceptionInfo)
  {
    std::string errcode;

    switch (ExceptionInfo->ExceptionRecord->ExceptionCode) {
      case EXCEPTION_ACCESS_VIOLATION:
        errcode = "Error: EXCEPTION_ACCESS_VIOLATION";
        break;
      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        errcode = "Error: EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
        break;
      case EXCEPTION_BREAKPOINT:
        errcode = "Error: EXCEPTION_BREAKPOINT";
        break;
      case EXCEPTION_DATATYPE_MISALIGNMENT:
        errcode = "Error: EXCEPTION_DATATYPE_MISALIGNMENT";
        break;
      case EXCEPTION_FLT_DENORMAL_OPERAND:
        errcode = "Error: EXCEPTION_FLT_DENORMAL_OPERAND";
        break;
      case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        errcode = "Error: EXCEPTION_FLT_DIVIDE_BY_ZERO";
        break;
      case EXCEPTION_FLT_INEXACT_RESULT:
        errcode = "Error: EXCEPTION_FLT_INEXACT_RESULT";
        break;
      case EXCEPTION_FLT_INVALID_OPERATION:
        errcode = "Error: EXCEPTION_FLT_INVALID_OPERATION";
        break;
      case EXCEPTION_FLT_OVERFLOW:
        errcode = "Error: EXCEPTION_FLT_OVERFLOW";
        break;
      case EXCEPTION_FLT_STACK_CHECK:
        errcode = "Error: EXCEPTION_FLT_STACK_CHECK";
        break;
      case EXCEPTION_FLT_UNDERFLOW:
        errcode = "Error: EXCEPTION_FLT_UNDERFLOW";
        break;
      case EXCEPTION_ILLEGAL_INSTRUCTION:
        errcode = "Error: EXCEPTION_ILLEGAL_INSTRUCTION";
        break;
      case EXCEPTION_IN_PAGE_ERROR:
        errcode = "Error: EXCEPTION_IN_PAGE_ERROR";
        break;
      case EXCEPTION_INT_DIVIDE_BY_ZERO:
        errcode = "Error: EXCEPTION_INT_DIVIDE_BY_ZERO";
        break;
      case EXCEPTION_INT_OVERFLOW:
        errcode = "Error: EXCEPTION_INT_OVERFLOW";
        break;
      case EXCEPTION_INVALID_DISPOSITION:
        errcode = "Error: EXCEPTION_INVALID_DISPOSITION";
        break;
      case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        errcode = "Error: EXCEPTION_NONCONTINUABLE_EXCEPTION";
        break;
      case EXCEPTION_PRIV_INSTRUCTION:
        errcode = "Error: EXCEPTION_PRIV_INSTRUCTION";
        break;
      case EXCEPTION_SINGLE_STEP:
        errcode = "Error: EXCEPTION_SINGLE_STEP";
        break;
      case EXCEPTION_STACK_OVERFLOW:
        errcode = "Error: EXCEPTION_STACK_OVERFLOW";
        break;
      default:
        errcode = "Error: Unrecognized Exception";
        break;
    }

    if (EXCEPTION_STACK_OVERFLOW != ExceptionInfo->ExceptionRecord->ExceptionCode) {

      auto handle = GetCurrentThread();
      LoggedStackWalker sw;
      sw.ShowCallstack(handle, ExceptionInfo->ContextRecord);
      auto base = std::string(std::format("Base address: 0x{:x}\n", GetBaseAddress()));
      sw.output_file << base;
      sw.output_file.close();

      std::string message("Oops, it's a crash!\n");
      message += errcode + "\n";
      message += "Crash log: " + sw.output_filename + "\n";
      message += base;

      MessageBoxA(nullptr, message.c_str(), "dfint hook error", MB_ICONERROR);
    } else {
      MessageBoxA(nullptr, "Stack Overflow!", "dfint hook error", MB_ICONERROR);
    }

    return EXCEPTION_EXECUTE_HANDLER;
  }

  void Install()
  {
    ::SetUnhandledExceptionFilter(Handler);
  }

}