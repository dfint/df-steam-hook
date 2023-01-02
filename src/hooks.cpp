#include "hooks.h"
#include "cache.hpp"
#include "dictionary.h"
#include "screen_manager.hpp"
#include "state_manager.hpp"
#include "ttf_manager.h"
#include "utils.hpp"

void* g_textures_ptr = nullptr;
graphicst_* g_graphics_ptr = nullptr;
std::atomic_flag ttf_injection_lock = ATOMIC_FLAG_INIT;

// cache for textures id
LRUCache<std::string, long> texture_id_cache(500);

// setup texture to texture vector and recieve tex_pos
SETUP_ORIG_FUNC(add_texture, 0xE827F0);
long __fastcall HOOK(add_texture)(void* ptr, SDL_Surface* texture)
{
  g_textures_ptr = ptr;
  auto texture_id = ORIGINAL(add_texture)(ptr, texture);
  // spdlog::debug("add texture id {}", ret);
  return texture_id;
}

// swap texture of specific char in chosen screen matrix (main/top)
template <auto T>
void InjectTTFChar(unsigned char symbol, int x, int y)
{
  auto tile = ScreenManager::GetSingleton()->GetTile<T>(x, y);

  // we use it only on tagert hooked functions, not for all tiles on screen
  if (ttf_injection_lock.test()) {
    std::string str(1, symbol);
    auto texture = TTFManager::GetSingleton()->CreateTexture(str);
    auto cached_texture_id = texture_id_cache.Get(str);
    long tex_pos = 0;
    if (cached_texture_id) {
      tex_pos = cached_texture_id.value().get();
      // spdlog::debug("texture id from cache {}", cached_texture_id.value().get());
    } else {
      tex_pos = ORIGINAL(add_texture)(g_textures_ptr, texture);
      texture_id_cache.Put(str, tex_pos);
      // spdlog::debug("new texture id {}", tex_pos);
    }
    tile->tex_pos = tex_pos;
  } else {
    tile->tex_pos = 0;
  }
}

// addchar used fot main windows chars drawing
SETUP_ORIG_FUNC(addchar, 0x55D80);
void __fastcall HOOK(addchar)(graphicst_* gps, unsigned char symbol, char advance)
{
  if (!ScreenManager::GetSingleton()->isInitialized() || g_textures_ptr == nullptr) {
    ORIGINAL(addchar)(gps, symbol, advance);
    return;
  }
  InjectTTFChar<ScreenManager::ScreenType::Main>(symbol, gps->screenx, gps->screeny);
  ORIGINAL(addchar)(gps, symbol, advance);
}

// addchar_top used for dialog windows
SETUP_ORIG_FUNC(addchar_top, 0xE9D60);
void __fastcall HOOK(addchar_top)(graphicst_* gps, unsigned char symbol, char advance)
{
  if (!ScreenManager::GetSingleton()->isInitialized() || g_textures_ptr == nullptr) {
    ORIGINAL(addchar_top)(gps, symbol, advance);
    return;
  }
  InjectTTFChar<ScreenManager::ScreenType::Top>(symbol, gps->screenx, gps->screeny);
  ORIGINAL(addchar_top)(gps, symbol, advance);
}

// main strings handling
SETUP_ORIG_FUNC(addst, 0x784C60);
void __fastcall HOOK(addst)(graphicst_* gps, DFString_* str, justification_ justify, int space)
{
  g_graphics_ptr = gps;

  std::string text;
  if (str->len > 15) {
    text = std::string(str->ptr);
  } else {
    text = std::string(str->buf);
  }

  // translation test segment

  // auto translation = Dictionary::GetSingleton()->Get(text);
  // if (translation) {
  //   auto cached = cache->Get(text);
  //   if (cached) {
  //     spdlog::debug("cache search {}", cached.value().get());
  //   }
  //   cache->Put(text, translation.value());

  //   // leak?
  //   DFString_ translated_str{};

  //   // just path does'n work
  //   // auto translated_len = translation.value().size();
  //   // str->len = translated_len;

  //   // if (translated_len > 15) {
  //   //   std::vector<char> cstr(translation.value().c_str(), translation.value().c_str() +
  //   translation.value().size()

  //   //   1); str->ptr = cstr.data(); str->capa = translated_len;
  //   // } else {
  //   //   str->pad = 0;
  //   //   str->capa = 15;
  //   //   strcpy(str->buf, translation.value().c_str());
  //   // }

  //   CreateDFString(translated_str, translation.value());

  //   ORIGINAL(addst)(gps, &translated_str, justify, space);
  //   return;
  // }

  ttf_injection_lock.test_and_set(std::memory_order_acquire);
  ORIGINAL(addst)(gps, str, justify, space);
  ttf_injection_lock.clear(std::memory_order_release);
}

// strings handling for dialog windows
SETUP_ORIG_FUNC(addst_top, 0x784DB0);
void __fastcall HOOK(addst_top)(graphicst_* gps, __int64 a2, __int64 a3)
{
  ttf_injection_lock.test_and_set(std::memory_order_acquire);
  ORIGINAL(addst_top)(gps, a2, a3);
  ttf_injection_lock.clear(std::memory_order_release);
}

// screen size can be catched here
SETUP_ORIG_FUNC(create_screen, 0x5BB540);
bool __fastcall HOOK(create_screen)(__int64 a1, unsigned int screen_width, unsigned int screen_height)
{
  spdlog::debug("create screen width {}, height {}, ptr 0x{:x}", screen_width, screen_height, (uintptr_t)a1);
  return ORIGINAL(create_screen)(a1, screen_width, screen_height);
}

// resizing font
// can be used to set current font size settings in ttfmanager
SETUP_ORIG_FUNC(reshape, 0x5C0930);
void __fastcall HOOK(reshape)(renderer_2d_base_* renderer, std::pair<int, int> max_grid)
{
  ORIGINAL(reshape)(renderer, max_grid);

  spdlog::debug("reshape dimx {} dimy {} dispx {} dispy {} dispx_z {} dispy_z {} screen 0x{:x}", renderer->dimx,
                renderer->dimy, renderer->dispx, renderer->dispy, renderer->dispx_z, renderer->dispy_z,
                (uintptr_t)renderer->screen);
}

// allocate screen array
SETUP_ORIG_FUNC(gps_allocate, 0x5C2AB0);
void __fastcall HOOK(gps_allocate)(void* ptr, int dimx, int dimy, int screen_width, int screen_height, int dispx_z,
                                   int dispy_z)
{

  spdlog::debug("gps allocate: dimx {} dimy {} screen_width {} screen_height {} dispx_z {} dispy_z {}", dimx, dimy,
                screen_width, screen_height, dispx_z, dispy_z);
  ORIGINAL(gps_allocate)(ptr, dimx, dimy, screen_width, screen_height, dispx_z, dispy_z);
  ScreenManager::GetSingleton()->AllocateScreen(dimx, dimy);
}

// clean screen array here
SETUP_ORIG_FUNC(cleanup_arrays, 0x5C28D0);
void __fastcall HOOK(cleanup_arrays)(void* ptr)
{
  ScreenManager::GetSingleton()->ClearScreen();
  ORIGINAL(cleanup_arrays)(ptr);
}

// render for main matrix
SETUP_ORIG_FUNC(screen_to_texid, 0x5BAB40);
Either<texture_fullid, texture_ttfid>* __fastcall HOOK(screen_to_texid)(renderer_* renderer, __int64 a2, int x, int y)
{
  Either<texture_fullid, texture_ttfid>* texture_by_id = ORIGINAL(screen_to_texid)(renderer, a2, x, y);
  if (ScreenManager::GetSingleton()->isInitialized() && g_graphics_ptr) {
    auto tile = ScreenManager::GetSingleton()->GetTile<ScreenManager::ScreenType::Main>(x, y);
    if (tile->tex_pos > 0) {
      texture_by_id->left.texpos = tile->tex_pos;
    }
  }
  return texture_by_id;
}

// renderer for top screen matrix
SETUP_ORIG_FUNC(screen_to_texid_top, 0x5BAD30);
Either<texture_fullid, texture_ttfid>* __fastcall HOOK(screen_to_texid_top)(renderer_* renderer, __int64 a2, int x,
                                                                            int y)
{
  Either<texture_fullid, texture_ttfid>* texture_by_id = ORIGINAL(screen_to_texid_top)(renderer, a2, x, y);
  if (ScreenManager::GetSingleton()->isInitialized() && g_graphics_ptr) {
    auto tile = ScreenManager::GetSingleton()->GetTile<ScreenManager::ScreenType::Top>(x, y);
    if (tile->tex_pos > 0) {
      texture_by_id->left.texpos = tile->tex_pos;
    }
  }
  return texture_by_id;
}

// loading main menu (start game)
SETUP_ORIG_FUNC(load_multi_pdim, 0xE82890);
void __fastcall HOOK(load_multi_pdim)(void* ptr, DFString_* filename, long* tex_pos, long dimx, long dimy,
                                      bool convert_magenta, long* disp_x, long* disp_y)
{
  // spdlog::debug("load_multi_pdim: filename {} text_pos {} dimx {} dimy {} convert_magenta {}, disp_x {} disp_y {}",
  //               filename->ptr, *tex_pos, dimx, dimy, convert_magenta, *disp_x, *disp_y);

  ORIGINAL(load_multi_pdim)(ptr, filename, tex_pos, dimx, dimy, convert_magenta, disp_x, disp_y);
}

// loading mods
SETUP_ORIG_FUNC(load_multi_pdim_2, 0xE82AD0);
void __fastcall HOOK(load_multi_pdim_2)(void* ptr, DFString_* filename, long* tex_pos, long dimx, long dimy,
                                        bool convert_magenta, long* disp_x, long* disp_y)
{
  // spdlog::debug("load_multi_pdim2: filename {} text_pos {} dimx {} dimy {} convert_magenta {}, disp_x {} disp_y {}",
  //               filename->ptr, *tex_pos, dimx, dimy, convert_magenta, *disp_x, *disp_y);
  // if we turn on cache here, game works... but during main menu stage it leaking
  // TODO: find new game/load game global texture reset

  ORIGINAL(load_multi_pdim_2)(ptr, filename, tex_pos, dimx, dimy, convert_magenta, disp_x, disp_y);
}

// game init
// not used
SETUP_ORIG_FUNC(main_init, 0x87A3C0);
void __fastcall HOOK(main_init)()
{
  ORIGINAL(main_init)();
  spdlog::debug("main init");
}

// catch Resetting textures in enabler asycn_wait loop
// not here, not used
SETUP_ORIG_FUNC(upload_textures, 0xE82020);
void __fastcall HOOK(upload_textures)(__int64 a1)
{
  ORIGINAL(upload_textures)(a1);
  // spdlog::debug("upload_textures");
}

// loading main
SETUP_ORIG_FUNC(loading_main, 0xA0CA70);
void* __fastcall HOOK(loading_main)(void* a1)
{
  return ORIGINAL(loading_main)(a1);
}

// loading_data_new_game_loop interface loop
SETUP_ORIG_FUNC(loading_world_new_game_loop, 0x9FD2E0);
void __fastcall HOOK(loading_world_new_game_loop)(void* a1)
{
  ORIGINAL(loading_world_new_game_loop)(a1);
  // offset of stage may changed!
  auto state = (int*)((uintptr_t)a1 + 292);
  if (*state >= 1 && *state <= 29) {
    StateManager::GetSingleton()->State(StateManager::Game);
  }
}

// loading_world_continuing_game_loop interface loop
SETUP_ORIG_FUNC(loading_world_continuing_game_loop, 0x566F40);
void __fastcall HOOK(loading_world_continuing_game_loop)(__int64 a1)
{
  ORIGINAL(loading_world_continuing_game_loop)(a1);
  // offset of stage may changed!
  auto state = (int*)((uintptr_t)a1 + 32);
  if (*state > 0 && *state < 50) {
    StateManager::GetSingleton()->State(StateManager::Game);
  }
}

// loading_world_start_new_game_loop interface loop
SETUP_ORIG_FUNC(loading_world_start_new_game_loop, 0x5652C0);
void __fastcall HOOK(loading_world_start_new_game_loop)(__int64 a1)
{
  ORIGINAL(loading_world_start_new_game_loop)(a1);
  // offset of stage may changed!
  auto state = (int*)((uintptr_t)a1 + 360);
  if (*state > 0 && *state < 34) {
    StateManager::GetSingleton()->State(StateManager::Game);
  }
}

// menu_interface_loop main menu interface loop
SETUP_ORIG_FUNC(menu_interface_loop, 0x1678A0);
void __fastcall HOOK(menu_interface_loop)(__int64 a1)
{
  ORIGINAL(menu_interface_loop)(a1);
  StateManager::GetSingleton()->State(StateManager::Menu);
}

void InstallHooks()
{
  // init TTFManager
  // should call init() for TTFInit and SDL function load from dll
  // then should load font for drawing text
  auto ttf = TTFManager::GetSingleton();
  ttf->Init();
  ttf->LoadFont("terminus_bold.ttf", 14, 2);

  // init StateManager, set callback to reset textures cache;
  auto state = StateManager::GetSingleton();
  state->SetCallback(StateManager::Menu, [&](void) { spdlog::debug("game state changed to StateManager::Menu"); });
  state->SetCallback(StateManager::Game, [&](void) {
    TTFManager::GetSingleton()->ClearCache();
    texture_id_cache.Clear();
    spdlog::debug("game state changed to StateManager::Game, clearing texture cache");
  });

  // ttf inject and translate here
  ATTACH(add_texture);
  ATTACH(addchar);
  ATTACH(addchar_top);
  ATTACH(addst);
  ATTACH(addst_top);
  ATTACH(screen_to_texid);
  ATTACH(screen_to_texid_top);
  ATTACH(gps_allocate);
  ATTACH(cleanup_arrays);

  // maybe scaling for bigger font pt?
  ATTACH(reshape);
  ATTACH(load_multi_pdim);
  ATTACH(load_multi_pdim_2);

  // game state tracking
  ATTACH(loading_world_new_game_loop);
  ATTACH(loading_world_continuing_game_loop);
  ATTACH(loading_world_start_new_game_loop);
  ATTACH(menu_interface_loop);

  // unused now
  ATTACH(create_screen);
  ATTACH(main_init);
  ATTACH(upload_textures);
  ATTACH(loading_main);

  spdlog::info("hooks installed");
}