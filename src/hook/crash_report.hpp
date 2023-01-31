#include "utils.hpp"

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

  std::string ErrCodeToString(DWORD code)
  {
    std::string errcode;

    switch (code) {
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

    return errcode;
  }

  LONG WINAPI Handler(EXCEPTION_POINTERS* ExceptionInfo)
  {
    std::string errcode = ErrCodeToString(ExceptionInfo->ExceptionRecord->ExceptionCode);

    if (EXCEPTION_STACK_OVERFLOW != ExceptionInfo->ExceptionRecord->ExceptionCode) {
      auto cr_filename = Config::Setting::crash_report_dir + "cr_" + Utils::now() + ".txt";
      auto cr_file = GetCrashReportLogHandle(cr_filename);

      cr_file << "Version: " << Config::Metadata::version << "\n";
      cr_file << "Checksum: " << std::format("0x{:x}", Config::Metadata::checksum) << "\n";
      cr_file << "Hook version: " << Config::Metadata::hook_version << "\n";
      cr_file << errcode << "\n";
      cr_file << "--------------Stack-------------\n";
      cr_file << std::stacktrace::current() << "\n";
      cr_file << "--------------------------------\n";

      std::string message("Oops, it's a crash!\n");
      message += errcode + "\n";
      message += "Crash log: " + cr_filename + "\n";

      MessageBoxA(nullptr, message.c_str(), "dfint hook error", MB_ICONERROR);
    } else {
      MessageBoxA(nullptr, "Stack Overflow!", "dfint hook error", MB_ICONERROR);
      return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_EXECUTE_HANDLER;
  }

  void Install()
  {
    ::SetUnhandledExceptionFilter(Handler);
  }

} // namespace CrashReport