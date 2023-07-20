#include "utils.hpp"

namespace CrashReport {

  constexpr const auto PATH_REPORTS = "./dfint_data/crash_reports/";

  std::ofstream GetCrashReportLogHandle(const std::string& filename) {
    if (!std::filesystem::is_directory(PATH_REPORTS) || !std::filesystem::exists(PATH_REPORTS)) {
      std::filesystem::create_directory(PATH_REPORTS);
    }

    std::ofstream file(filename);
    return file;
  }

  std::string ErrCodeToString(DWORD code) {
    switch (code) {
      case EXCEPTION_ACCESS_VIOLATION:
        return "Error: EXCEPTION_ACCESS_VIOLATION";
      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
        return "Error: EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
      case EXCEPTION_BREAKPOINT:
        return "Error: EXCEPTION_BREAKPOINT";
      case EXCEPTION_DATATYPE_MISALIGNMENT:
        return "Error: EXCEPTION_DATATYPE_MISALIGNMENT";
      case EXCEPTION_FLT_DENORMAL_OPERAND:
        return "Error: EXCEPTION_FLT_DENORMAL_OPERAND";
      case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        return "Error: EXCEPTION_FLT_DIVIDE_BY_ZERO";
      case EXCEPTION_FLT_INEXACT_RESULT:
        return "Error: EXCEPTION_FLT_INEXACT_RESULT";
      case EXCEPTION_FLT_INVALID_OPERATION:
        return "Error: EXCEPTION_FLT_INVALID_OPERATION";
      case EXCEPTION_FLT_OVERFLOW:
        return "Error: EXCEPTION_FLT_OVERFLOW";
      case EXCEPTION_FLT_STACK_CHECK:
        return "Error: EXCEPTION_FLT_STACK_CHECK";
      case EXCEPTION_FLT_UNDERFLOW:
        return "Error: EXCEPTION_FLT_UNDERFLOW";
      case EXCEPTION_ILLEGAL_INSTRUCTION:
        return "Error: EXCEPTION_ILLEGAL_INSTRUCTION";
      case EXCEPTION_IN_PAGE_ERROR:
        return "Error: EXCEPTION_IN_PAGE_ERROR";
      case EXCEPTION_INT_DIVIDE_BY_ZERO:
        return "Error: EXCEPTION_INT_DIVIDE_BY_ZERO";
      case EXCEPTION_INT_OVERFLOW:
        return "Error: EXCEPTION_INT_OVERFLOW";
      case EXCEPTION_INVALID_DISPOSITION:
        return "Error: EXCEPTION_INVALID_DISPOSITION";
      case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        return "Error: EXCEPTION_NONCONTINUABLE_EXCEPTION";
      case EXCEPTION_PRIV_INSTRUCTION:
        return "Error: EXCEPTION_PRIV_INSTRUCTION";
      case EXCEPTION_SINGLE_STEP:
        return "Error: EXCEPTION_SINGLE_STEP";
      case EXCEPTION_STACK_OVERFLOW:
        return "Error: EXCEPTION_STACK_OVERFLOW";
      default:
        return "Error: Unrecognized Exception";
    }
  }

  LONG WINAPI Handler(EXCEPTION_POINTERS* ExceptionInfo) {
    const std::string& errcode = ErrCodeToString(ExceptionInfo->ExceptionRecord->ExceptionCode);

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
      cr_file.close();

      auto message = std::format("Oops, it's a crash!\n{}\nCrash log: {}\n", errcode, cr_filename);
      MessageBoxA(nullptr, message.c_str(), "dfint hook error", MB_ICONERROR);
    } else {
      MessageBoxA(nullptr, "Stack Overflow!", "dfint hook error", MB_ICONERROR);
      return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_EXECUTE_HANDLER;
  }

  void Install() {
    ::SetUnhandledExceptionFilter(Handler);
  }

} // namespace CrashReport