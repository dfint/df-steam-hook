#pragma once

// #include <regex> // remove if regex dont needed
#include <atomic>
#include <codecvt> // for std::codecvt_utf8 - myabe not needed cause we can render utf8
#include <cstddef>
#include <format>
#include <fstream>
#include <list>
#include <locale> // for std::wstring_convert - myabe not needed cause we can render utf8
#include <set>
#include <stdexcept>
#include <thread>
#include <unordered_map>

/* disable unused headers from Windows.h */
#define WIN32_LEAN_AND_MEAN
#define NOSOUND          // Sound driver routines
#define NOTEXTMETRIC     // typedef TEXTMETRIC and associated routines
#define NOWH             // SetWindowsHook and WH_*
#define NOWINOFFSETS     // GWL_*, GCL_*, associated routines
#define NOCOMM           // COMM driver routines
#define NOKANJI          // Kanji support stuff.
#define NOHELP           // Help engine interface.
#define NOPROFILER       // Profiler interface.
#define NODEFERWINDOWPOS // DeferWindowPos routines
#define NOMCX            // Modem Configuration Extensions
#define NOSYSMETRICS     // SM_*
#define NOMENUS          // MF_*
#define NOICONS          // IDI_*
#define NOKEYSTATES      // MK_*
#define NOSYSCOMMANDS    // SC_*
#define NORASTEROPS      // Binary and Tertiary raster ops
#define OEMRESOURCE      // OEM Resource values
#define NOATOM           // Atom Manager routines
#define NOCLIPBOARD      // Clipboard routines
#define NOCOLOR          // Screen colors
#define NODRAWTEXT       // DrawText() and DT_*
#define NOGDI            // All GDI defines and routines
#define NOKERNEL         // All KERNEL defines and routines
#define NOMEMMGR         // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE       // typedef METAFILEPICT
#define NOMINMAX         // Macros min(a,b) and max(a,b)
#define NOOPENFILE       // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL         // SB_* and scrolling routines
#define NOSERVICE        // All Service Controller routines, SERVICE_ equates, etc.
#include <windows.h>

#include "SDL/SDL_ttf.h"
#pragma comment(lib, "SDL_ttf.lib")

#include <detours/detours.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include <toml++/toml.h>

#include "config.h"
