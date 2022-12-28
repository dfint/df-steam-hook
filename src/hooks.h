
enum justification : unsigned char
{
  justify_left,
  justify_center,
  justify_right,
  justify_cont,
  not_truetype
};

struct string_
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
  char screenf, screenb;
  char screenbright;
  char dump2[336];
  unsigned char* screen;
  char dump3[1028];
  int dimx, dimy;
};

#define SETUP_ORIG_FUNC(fn_name, shift)                                        \
  fn_name fn_name##_orig = (fn_name)((UINT64)GetModuleHandle(0) + shift);

#define SETUP_ORIG_FUNC_FNAME(fn_name, module_name)                            \
  fn_name fn_name##_orig =                                                     \
    (fn_name)(GetProcAddress(GetModuleHandle(#module_name), #fn_name));

#define GET_ADDR(shift) (PVOID)((UINT64)GetModuleHandle(0) + shift)

#define ATTACH(fn_name)                                                        \
  DetourAttach(&(PVOID&)(fn_name##_orig), (PVOID)fn_name##_hook)

#define HOOK(fn_name) fn_name##_hook
#define ORIGINAL(fn_name) fn_name##_orig

typedef long(__fastcall* add_texture)(void* ptr, void* a2);
typedef void(__fastcall* addst)(graphicst_* gps, string_* str,
                                unsigned __int8 just, int space);
typedef void(__fastcall* cleanup_arrays)(void* ptr);
typedef void(__fastcall* gps_allocate)(void* ptr, int a2, int a3, int a4,
                                       int a5, int a6, int a7);
typedef bool(__fastcall* create_screen)(__int64 a1, unsigned int width,
                                        unsigned int height);

void InstallHooks();