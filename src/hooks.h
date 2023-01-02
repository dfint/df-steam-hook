#pragma once

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

struct DFString_
{
  union
  {
    char buf[16];
    char* ptr;
  };
  unsigned __int64 len;
  unsigned __int64 capa;
  __int64 pad;
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
  uint8_t* screen;
  char dump3[1028];
  int32_t dimx;
  int32_t dimy;
};

struct renderer_2d_base_
{
  char dump[240];
  SDL_Surface* screen;          // 8
  char pad16[16];               // 16
  int32_t dispx;                // 4
  int32_t dispy;                // 4
  int32_t dimx;                 // 4
  int32_t dimy;                 // 4
  int32_t dispx_z;              // 4
  int32_t dispy_z;              // 4
  int32_t origin_x;             // 4
  int32_t origin_y;             // 4
  bool use_viewport_zoom;       // 1
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

#define SETUP_ORIG_FUNC(fn_name, shift) fn_name fn_name##_orig = (fn_name)((UINT64)GetModuleHandle(0) + shift);
#define SETUP_ORIG_FUNC_FNAME(fn_name, module_name)                                                                    \
  fn_name fn_name##_orig = (fn_name)(GetProcAddress(GetModuleHandle(#module_name), #fn_name));
#define GET_ADDR(shift) (PVOID)((UINT64)GetModuleHandle(0) + shift)
#define ATTACH(fn_name) DetourAttach(&(PVOID&)(fn_name##_orig), (PVOID)fn_name##_hook)
#define HOOK(fn_name) fn_name##_hook
#define ORIGINAL(fn_name) fn_name##_orig

typedef long(__fastcall* add_texture)(void* ptr, void* a2);
typedef void(__fastcall* addst)(graphicst_* gps, DFString_* str, unsigned __int8 just, int space);
typedef void(__fastcall* addchar)(graphicst_* gps, unsigned char a2, char a3);
typedef void(__fastcall* cleanup_arrays)(void* ptr);
typedef void(__fastcall* gps_allocate)(void* ptr, int a2, int a3, int a4, int a5, int a6, int a7);
typedef bool(__fastcall* create_screen)(__int64 a1, unsigned int width, unsigned int height);
typedef void(__fastcall* reshape)(renderer_2d_base_* ptr, std::pair<int, int> max_grid);
typedef void(__fastcall* load_multi_pdim)(void* ptr, DFString_* filename, long* tex_pos, long dimx, long dimy,
                                          bool convert_magenta, long* disp_x, long* disp_y);
typedef void(__fastcall* load_multi_pdim_2)(void* ptr, DFString_* filename, long* tex_pos, long dimx, long dimy,
                                            bool convert_magenta, long* disp_x, long* disp_y);
typedef void(__fastcall* gps_allocate)(void* ptr, int a2, int a3, int a4, int a5, int a6, int a7);
typedef void(__fastcall* cleanup_arrays)(void* ptr);
typedef Either<texture_fullid, texture_ttfid>*(__fastcall* screen_to_texid)(renderer_* a1, __int64 a2, int a3, int a4);
typedef void(__fastcall* main_init)();
typedef void(__fastcall* upload_textures)(__int64 a1);
typedef void(__fastcall* loading_world_new_game_loop)(void* a1);
typedef void(__fastcall* loading_world_continuing_game_loop)(__int64 a1);
typedef void(__fastcall* loading_world_start_new_game_loop)(__int64 a1);
typedef void(__fastcall* menu_interface_loop)(__int64 a1);
typedef void*(__fastcall* loading_main)(void* a1);

void InstallHooks();