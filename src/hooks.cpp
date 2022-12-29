#include "hooks.h"
#include "cache.hpp"
#include "dictionary.h"
#include "ttf_manager.h"

// int char_height = 12;
// int char_width = 8;

void* g_textures_ptr = nullptr;
graphicst_* g_graphics_ptr = nullptr;
unsigned long* my_screen = nullptr;

// screen screen array
ScreenCharTile* screen_ptr = nullptr;

// const after creation
SDL_Surface* SAMPLE = nullptr;

LRUCache<std::string, std::string> cache(100);

// with ttf this hook unneeded
// not used now
SETUP_ORIG_FUNC(add_texture, 0xE827F0);
long __fastcall HOOK(add_texture)(void* ptr, void* a2)
{
  g_textures_ptr = ptr;
  return ORIGINAL(add_texture)(ptr, a2);
}

// string can be catched here
SETUP_ORIG_FUNC(addst, 0x784C60);
void __fastcall HOOK(addst)(graphicst_* gps, string_* str, justification_ justify, int space)
{
  g_graphics_ptr = gps;

  std::string text;
  if (str->len > 15) {
    text = std::string(str->ptr);
  } else {
    text = std::string(str->buf);
  }

  auto translated = Dictionary::GetSingleton()->Get(text);
  if (translated) {
    auto c = cache.Get(text);
    if (c != nullptr) {
      spdlog::debug("cache search {}", *c);
    }
    cache.Put(text, translated.value());
  }

  // if (my_screen != nullptr && g_textures_ptr != nullptr) {
  //   for (int i = 0; i < text.size(); i++) {
  //     long tex_pos = ORIGINAL(add_texture)(g_textures_ptr, SAMPLE);
  //     unsigned long* s = my_screen + (gps->screenx + i) * gps->dimy * 4 + gps->screeny * 4 + 4;
  //     *s = tex_pos;

  //     // auto* s = screen_ptr + (gps->screenx + i) * gps->dimy + gps->screeny;
  //     // s->tex_pos = tex_pos;
  //   }
  // }

  ORIGINAL(addst)(gps, str, justify, space);
  return;
}

// screen size can be catched here
// not used for now
SETUP_ORIG_FUNC(create_screen, 0x5BB540);
bool __fastcall HOOK(create_screen)(__int64 a1, unsigned int width, unsigned int height)
{
  spdlog::debug("create screen width {}, height {}, ptr 0x{:x}", width, height, (uintptr_t)a1);
  return ORIGINAL(create_screen)(a1, width, height);
}

// resizing font
// can be used to set current font size in ttfmanager
SETUP_ORIG_FUNC(reshape, 0x5C0930);
void __fastcall HOOK(reshape)(renderer_2d_base_* renderer, std::pair<int, int> max_grid)
{
  ORIGINAL(reshape)(renderer, max_grid);

  // char_height = renderer->dispy_z;
  // char_width = renderer->dispx_z;
  // TTFManager::GetSingleton()->LoadFont("terminus_bold.ttf", char_height);
  // resize font to ptr->dipsx/y
  spdlog::debug("reshape dimx {} dimy {} dispx {} dispy {} dispx_z {} dispy_z {} screen 0x{:x}", renderer->dimx,
                renderer->dimy, renderer->dispx, renderer->dispy, renderer->dispx_z, renderer->dispy_z,
                (uintptr_t)renderer->screen);
}

// allocate screen array, get ptr here
SETUP_ORIG_FUNC(gps_allocate, 0x5C2AB0);
void __fastcall HOOK(gps_allocate)(void* ptr, int a2, int a3, int a4, int a5, int a6, int a7)
{

  spdlog::debug("gps allocate: {} {} {} {} {} {}", a2, a3, a4, a5, a6, a7);
  ORIGINAL(gps_allocate)(ptr, a2, a3, a4, a5, a6, a7);

  my_screen = new unsigned long[a2 * a3 * 12];
  // screen_ptr = new ScreenCharTile[a2 * a3 * 4];
}

// clean screen array here
SETUP_ORIG_FUNC(cleanup_arrays, 0x5C28D0);
void __fastcall HOOK(cleanup_arrays)(void* ptr)
{
  if (my_screen) {
    delete[] my_screen;
  }
  my_screen = nullptr;

  // if (screen_ptr) {
  //   delete[] screen_ptr;
  // }
  // screen_ptr = nullptr;

  ORIGINAL(cleanup_arrays)(ptr);
}

SETUP_ORIG_FUNC(screen_to_texid, 0x5BAB40);
Either<texture_fullid, texture_ttfid>* __fastcall HOOK(screen_to_texid)(renderer_* a1, __int64 a2, int a3, int a4)
{

  Either<texture_fullid, texture_ttfid>* ret = ORIGINAL(screen_to_texid)(a1, a2, a3, a4);

  if (my_screen && g_graphics_ptr) {
    const int tile = a3 * g_graphics_ptr->dimy + a4;
    const unsigned long* s = my_screen + tile * 4 + 4;
    if (s[0] > 0) {
      // printf("%d %d %d\n", a3, a4, s[0]);
      ret->left.texpos = s[0];
    }
  }

  // if (screen_ptr && g_graphics_ptr) {
  //   const int tile = a3 * g_graphics_ptr->dimy + a4;
  //   const auto* s = screen_ptr + tile;
  //   if (s->tex_pos > 0) {
  //     // printf("%d %d %d\n", a3, a4, s[0]);
  //     ret->left.texpos = s->tex_pos;
  //   }
  // }

  return ret;
}

void InstallHooks()
{
  // init TTFManager
  // should call init() for TTFInit and SDL function load from dll
  // then should load font for drawing text
  auto ttf = TTFManager::GetSingleton();
  ttf->Init();
  ttf->LoadFont("terminus_bold.ttf", 14);

  // erase callback test
  cache.SetEraseCallback(
    [](const std::string& key, const std::string& value) { spdlog::debug("callback key {} value {}", key, value); });

  // temp tex for testing
  SAMPLE = ttf->CreateTexture("Я").texture;

  ATTACH(add_texture);
  ATTACH(addst);
  ATTACH(create_screen);
  ATTACH(reshape);
  ATTACH(screen_to_texid);
  ATTACH(gps_allocate);
  ATTACH(cleanup_arrays);

  spdlog::info("hooks installed");
}