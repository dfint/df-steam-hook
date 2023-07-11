#pragma once

#include "keybindings.h"
namespace Hooks {

  enum curses_color_ : uint8_t
  {
    Black,
    Blue,
    Green,
    Cyan,
    Red,
    Magenta,
    Yellow,
    White,
  };

  enum justification_ : uint8_t
  {
    justify_left,
    justify_center,
    justify_right,
    justify_cont,
    not_truetype
  };

  struct graphicst_
  {
    char dump[129];
    long screenx;
    long screeny;
    curses_color_ screenf;
    curses_color_ screenb;
    bool screenbright;
    char dump2[336];
    uint8_t* screen;                   // 8 default 1028 to dimx/y
    uint8_t* screen_limit;             // 8
    long* screentexpos;                // 8
    long* screentexpos_lower;          // 8
    long* screentexpos_anchored;       // 8
    long* screentexpos_anchored_x;     // 8
    long* screentexpos_anchored_y;     // 8
    uint32_t* screentexpos_flag;       // 8
    bool top_in_use;                   // 1 to 8
    uint8_t* screen_top;               // 8
    uint8_t* screen_limit_top;         // 8
    long* screentexpos_top_lower;      // 8
    long* screentexpos_top_anchored;   // 8
    long* screentexpos_top;            // 8
    long* screentexpos_top_anchored_x; // 8
    long* screentexpos_top_anchored_y; // 8
    uint32_t* screentexpos_top_flag;   // 8
    char dump3[900];
    int32_t dimx;
    int32_t dimy;
  };

  struct renderer_2d_base_
  {
    char dump[240];
    // SDL_Surface* screen;          // 8
    void* screen;                 // 8
    char pad16[16];               // 16
    int32_t dispx;                // 4
    int32_t dispy;                // 4
    int32_t dimx;                 // 4
    int32_t dimy;                 // 4
    int32_t dispx_z;              // 4
    int32_t dispy_z;              // 4
    int32_t origin_x;             // 4
    int32_t origin_y;             // 4
    bool use_viewport_zoom;       // 1 to 4
    int32_t viewport_zoom_factor; // 4
    char pad24[24];               // 24
    int32_t zoom_steps;           // 4
    int32_t forced_steps;         // 4
    int32_t natural_w;            // 4
    int32_t natural_h;            // 4
  };

  class renderer_
  {
  public:
    unsigned char* screen;
    long* screentexpos;
    char* screentexpos_addcolor;
    unsigned char* screentexpos_grayscale;
    unsigned char* screentexpos_cf;
    unsigned char* screentexpos_cbr;
    // For partial printing:
    unsigned char* screen_old;
    long* screentexpos_old;
    char* screentexpos_addcolor_old;
    unsigned char* screentexpos_grayscale_old;
    unsigned char* screentexpos_cf_old;
    unsigned char* screentexpos_cbr_old;
  };

  struct texture_fullid
  {
    int texpos;
    float r, g, b;
    float br, bg, bb;

    bool operator<(const struct texture_fullid& other) const
    {
      if (texpos != other.texpos)
        return texpos < other.texpos;
      if (r != other.r)
        return r < other.r;
      if (g != other.g)
        return g < other.g;
      if (b != other.b)
        return b < other.b;
      if (br != other.br)
        return br < other.br;
      if (bg != other.bg)
        return bg < other.bg;
      return bb < other.bb;
    }
  };

  typedef int texture_ttfid; // Just the texpos

  template <typename L, typename R>
  struct Either
  {
    bool isL;
    union
    {
      L left;
      R right;
    };
    Either(const L& l)
    {
      isL = true;
      left = l;
    }
    Either(const R& r)
    {
      isL = false;
      right = r;
    }
  };

  inline auto module_handle = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

#define SETUP_ORIG_FUNC_OFFSET(fn_name, shift) auto fn_name##_orig = reinterpret_cast<fn_name>(module_handle + shift);
#define SETUP_ORIG_FUNC(fn_name)                                                                                       \
  auto fn_name##_orig = reinterpret_cast<fn_name>(module_handle + Config::Offset::fn_name);
#define SETUP_ORIG_FUNC_FNAME(fn_name, module_name)                                                                    \
  auto fn_name##_orig = reinterpret_cast<fn_name>(GetProcAddress(GetModuleHandle(#module_name), #fn_name));
#define GET_ADDR(shift) reinterpret_cast<void*>(module_handle + shift)
#define ATTACH(fn_name) DetourAttach(&(reinterpret_cast<void*&>(fn_name##_orig)), (void*)fn_name##_hook);
#define DETACH(fn_name) DetourDetach(&(reinterpret_cast<void*&>(fn_name##_orig)), (void*)fn_name##_hook);
#define HOOK(fn_name) fn_name##_hook
#define ORIGINAL(fn_name) fn_name##_orig

  // translation
  typedef char*(__cdecl* string_copy)(char* dst, const char* src);
  typedef char*(__cdecl* string_copy_n)(char* dst, const char* src, size_t size);
  typedef char*(__cdecl* string_append)(char* dst, const char* src);
  typedef char*(__cdecl* string_append_0)(char* dst, const char* src);
  typedef char*(__cdecl* string_append_n)(char* dts, const char* src, size_t size);
  typedef void(__fastcall* convert_ulong_to_string)(uint32_t n, std::string& str);
  typedef void(__fastcall* addst)(graphicst_* gps, std::string& str, unsigned __int8 just, int space);
  typedef void(__fastcall* addst_top)(graphicst_* gps, std::string& str, __int64 a3);
  typedef void(__fastcall* addcoloredst)(graphicst_* gps, const char* str, __int64 a3);
  typedef void(__fastcall* addst_flag)(graphicst_* a1, std::string& str, __int64 a3, __int64 a4, int a5);

  // search
  typedef int(__fastcall* standardstringentry)(std::string& str, int maxlen, unsigned int flag,
                                               std::set<InterfaceKey>& events, uint16_t* utf);
  typedef void(__fastcall* upper_case_string)(std::string& str);
  typedef void(__fastcall* simplify_string)(std::string& str);
  typedef void(__fastcall* lower_case_string)(std::string& str);
  typedef void(__fastcall* capitalize_string_words)(std::string& str);
  typedef void(__fastcall* capitalize_string_first_word)(std::string& str);

  // ttf inject
  typedef long(__fastcall* add_texture)(void* ptr, void* a2);
  typedef void(__fastcall* addchar)(graphicst_* gps, unsigned char symbol, char a3);
  typedef void(__fastcall* addchar_top)(graphicst_* gps, unsigned char symbol, char advance);
  typedef void(__fastcall* gps_allocate)(void* ptr, int a2, int a3, int a4, int a5, int a6, int a7);
  typedef void(__fastcall* cleanup_arrays)(void* ptr);
  typedef Either<texture_fullid, texture_ttfid>*(__fastcall* screen_to_texid)(renderer_* a1, __int64 a2, int a3,
                                                                              int a4);
  typedef Either<texture_fullid, texture_ttfid>*(__fastcall* screen_to_texid_top)(renderer_* a1, __int64 a2, int a3,
                                                                                  int a4);

  // game state
  typedef void(__fastcall* loading_world_new_game_loop)(void* a1);
  typedef void(__fastcall* loading_world_continuing_game_loop)(__int64 a1);
  typedef void(__fastcall* loading_world_start_new_game_loop)(__int64 a1);
  typedef void(__fastcall* menu_interface_loop)(__int64 a1);

  void InstallTranslation();
  void UninstallTranslation();
  void InstallTTFInjection();
  void UninstallTTFInjection();
  void InstallStateManager();
  void UninstallStateManager();

}