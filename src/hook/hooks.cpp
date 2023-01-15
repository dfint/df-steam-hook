#include "hooks.h"
#include "cache.hpp"
#include "dictionary.h"
#include "screen_manager.hpp"
#include "state_manager.hpp"
// #include "ttf_manager.h"

namespace Hook {

  void* g_textures_ptr = nullptr;
  graphicst_* g_graphics_ptr = nullptr;
  std::atomic_flag ttf_injection_lock = ATOMIC_FLAG_INIT;
  std::atomic_flag converter_lock = ATOMIC_FLAG_INIT;

  // cache for textures id
  LRUCache<std::string, long> texture_id_cache(500);

  // setup texture to texture vector and recieve tex_pos
  // SETUP_ORIG_FUNC(add_texture);
  // long __fastcall HOOK(add_texture)(void* ptr, SDL_Surface* texture)
  // {
  //   g_textures_ptr = ptr;
  //   return ORIGINAL(add_texture)(ptr, texture);
  // }

  template <typename T, typename... Args>
  void LockedInject(T& func, Args&&... args)
  {
    ttf_injection_lock.test_and_set(std::memory_order_acquire);
    func(args...);
    ttf_injection_lock.clear(std::memory_order_release);
  }

  // swap texture of specific char in chosen screen matrix (main/top)
  template <auto T>
  void InjectTTFChar(unsigned char symbol, int x, int y)
  {
    if (ttf_injection_lock.test())
      return;

    auto tile = ScreenManager::GetSingleton()->GetTile<T>(x, y);

    if (symbol > 0) {
      std::string str(1, symbol);
      // auto texture = TTFManager::GetSingleton()->CreateTexture(str);
      auto cached_texture_id = texture_id_cache.Get(str);
      long tex_pos = 0;
      if (cached_texture_id) {
        tex_pos = cached_texture_id.value().get();
      } else {
        // tex_pos = ORIGINAL(add_texture)(g_textures_ptr, texture);
        texture_id_cache.Put(str, tex_pos);
      }
      tile->tex_pos = tex_pos;
    } else {
      tile->tex_pos = 0;
    }
  }

  // addchar used fot main windows chars drawing
  SETUP_ORIG_FUNC(addchar);
  void __fastcall HOOK(addchar)(graphicst_* gps, wchar_t symbol, char advance)
  {
    g_graphics_ptr = gps;
    if (ScreenManager::GetSingleton()->isInitialized() && g_textures_ptr != nullptr) {
      // InjectTTFChar<ScreenManager::ScreenType::Main>(symbol, gps->screenx, gps->screeny);
    }
    ORIGINAL(addchar)(gps, symbol, advance);
  }

  // addchar_top used for dialog windows
  SETUP_ORIG_FUNC(addchar_top);
  void __fastcall HOOK(addchar_top)(graphicst_* gps, wchar_t symbol, char advance)
  {
    if (ScreenManager::GetSingleton()->isInitialized() && g_textures_ptr != nullptr) {
      // InjectTTFChar<ScreenManager::ScreenType::Top>(symbol, gps->screenx, gps->screeny);
    }
    ORIGINAL(addchar_top)(gps, symbol, advance);
  }

  // allocate screen array
  SETUP_ORIG_FUNC(gps_allocate);
  void __fastcall HOOK(gps_allocate)(void* ptr, int dimx, int dimy, int screen_width, int screen_height, int dispx_z,
                                     int dispy_z)
  {

    logger::debug("gps allocate: dimx {} dimy {} screen_width {} screen_height {} dispx_z {} dispy_z {}", dimx, dimy,
                  screen_width, screen_height, dispx_z, dispy_z);
    ORIGINAL(gps_allocate)(ptr, dimx, dimy, screen_width, screen_height, dispx_z, dispy_z);
    ScreenManager::GetSingleton()->AllocateScreen(dimx, dimy);
  }

  // clean screen array here
  SETUP_ORIG_FUNC(cleanup_arrays);
  void __fastcall HOOK(cleanup_arrays)(void* ptr)
  {
    ScreenManager::GetSingleton()->ClearScreen<ScreenManager::ScreenType::Main>();
    ScreenManager::GetSingleton()->ClearScreen<ScreenManager::ScreenType::Top>();
    ORIGINAL(cleanup_arrays)(ptr);
  }

  // render for main matrix
  SETUP_ORIG_FUNC(screen_to_texid);
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
  SETUP_ORIG_FUNC(screen_to_texid_top);
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

  // main strings handling
  // ttf swap option
  // SETUP_ORIG_FUNC(addst, 0x784C60);
  // void __fastcall HOOK(addst)(graphicst_* gps, std::string& str, justification_ justify, int space)
  // {
  //   // translation test segment
  //   // logger::debug("addst text {} len {} capa {}", text, str->len, str->capa);

  //   auto translation = Dictionary::GetSingleton()->Get(str);
  //   if (translation) {
  //     auto tstr = translation.value();
  //     auto wstr = s2ws(translation.value());
  //     for (int i = 0; i < wstr.size(); i++) {
  //       InjectTTFChar<ScreenManager::ScreenType::Main>(wstr[i], gps->screenx + i, gps->screeny);
  //     }
  //     tstr.resize(wstr.size());
  //     LockedInject(ORIGINAL(addst), gps, tstr, justify, space);
  //     return;
  //   }

  //   ORIGINAL(addst)(gps, str, justify, space);
  // }

  // strcpy
  SETUP_ORIG_FUNC(string_copy);
  char* __cdecl HOOK(string_copy)(char* dst, const char* src)
  {
    if (src && dst && Config::Setting::enable_translation) {
      auto tstr = Dictionary::GetSingleton()->Get(src);
      if (tstr) {
        return ORIGINAL(string_copy)(dst, tstr.value().c_str());
      }
    }
    return ORIGINAL(string_copy)(dst, src);
  }

  // strncpy
  SETUP_ORIG_FUNC(string_copy_n);
  char* __cdecl HOOK(string_copy_n)(char* dst, const char* src, size_t size)
  {
    if (src && dst && size && Config::Setting::enable_translation) {
      auto tstr = Dictionary::GetSingleton()->Get(src);
      if (tstr) {
        return ORIGINAL(string_copy_n)(dst, tstr.value().c_str(), tstr.value().size());
      }
    }
    return ORIGINAL(string_copy_n)(dst, src, size);
  }

  // strcat
  SETUP_ORIG_FUNC(string_append);
  char* __cdecl HOOK(string_append)(char* dst, const char* src)
  {
    if (src && dst && Config::Setting::enable_translation) {
      auto tstr = Dictionary::GetSingleton()->Get(src);
      if (tstr) {
        return ORIGINAL(string_append)(dst, tstr.value().c_str());
      }
    }
    return ORIGINAL(string_append)(dst, src);
  }

  // strcat_0
  SETUP_ORIG_FUNC(string_append_0);
  char* __cdecl HOOK(string_append_0)(char* dst, const char* src)
  {
    if (src && dst && Config::Setting::enable_translation) {
      auto tstr = Dictionary::GetSingleton()->Get(src);
      if (tstr) {
        return ORIGINAL(string_append_0)(dst, tstr.value().c_str());
      }
    }
    return ORIGINAL(string_append_0)(dst, src);
  }

  // strncat
  SETUP_ORIG_FUNC(string_append_n);
  char* __cdecl HOOK(string_append_n)(char* dst, const char* src, size_t size)
  {
    if (src && dst && size && Config::Setting::enable_translation) {
      auto tstr = Dictionary::GetSingleton()->Get(src);
      if (tstr) {
        return ORIGINAL(string_append_n)(dst, tstr.value().c_str(), tstr.value().size());
      }
    }
    return ORIGINAL(string_append_n)(dst, src, size);
  }

  // convert_ulong_to_string
  // do not call original!
  SETUP_ORIG_FUNC(convert_ulong_to_string);
  void __fastcall HOOK(convert_ulong_to_string)(uint32_t n, std::string& str)
  {
    std::ostringstream o;
    if (n > 4000000000) {
      auto n1 = n - 4294967296;
      o << n1;
      str = o.str();
      return;
    }
    o << n;
    str = o.str();
  }

  SETUP_ORIG_FUNC(addst);
  void __fastcall HOOK(addst)(graphicst_* gps, std::string& str, justification_ justify, int space)
  {
    if (gps && !str.empty() && Config::Setting::enable_translation) {
      auto translation = Dictionary::GetSingleton()->Get(str);
      if (translation) {
        return ORIGINAL(addst)(gps, translation.value(), justify, space);
      }
    }
    ORIGINAL(addst)(gps, str, justify, space);
  }

  // strings handling for dialog windows (top screen matrix)
  SETUP_ORIG_FUNC(addst_top);
  void __fastcall HOOK(addst_top)(graphicst_* gps, std::string& str, __int64 a3)
  {
    if (gps && !str.empty() && Config::Setting::enable_translation) {
      auto translation = Dictionary::GetSingleton()->Get(str);
      if (translation) {
        return ORIGINAL(addst_top)(gps, translation.value(), a3);
      }
    }
    ORIGINAL(addst_top)(gps, str, a3);
  }

  // some colored string with color not from enum
  // not see it
  SETUP_ORIG_FUNC(addcoloredst);
  void __fastcall HOOK(addcoloredst)(graphicst_* gps, const char* str, __int64 a3)
  {
    auto len = strnlen_s(str, 1000);
    if (gps && str && len > 0 && len < 1000 && Config::Setting::enable_translation) {
      auto translation = Dictionary::GetSingleton()->Get(str);
      if (translation) {
        return ORIGINAL(addcoloredst)(gps, translation.value().c_str(), a3);
      }
    }
    ORIGINAL(addcoloredst)(gps, str, a3);
  }

  // render through different procedure, not like addst or addst_top
  SETUP_ORIG_FUNC(addst_flag);
  void __fastcall HOOK(addst_flag)(graphicst_* gps, std::string& str, __int64 a3, __int64 a4, int some_flag)
  {
    if (gps && !str.empty() && Config::Setting::enable_translation) {
      auto translation = Dictionary::GetSingleton()->Get(str);
      if (translation) {
        return ORIGINAL(addst_flag)(gps, translation.value(), a3, a4, some_flag);
      }
    }
    ORIGINAL(addst_flag)(gps, str, a3, a4, some_flag);
  }

  // // dynamic template string
  SETUP_ORIG_FUNC(addst_template);
  void __fastcall HOOK(addst_template)(renderer_2d_base_* renderer, std::string& str)
  {
    ORIGINAL(addst_template)(renderer, str);
  }

  // loading_data_new_game_loop interface loop
  SETUP_ORIG_FUNC(loading_world_new_game_loop);
  void __fastcall HOOK(loading_world_new_game_loop)(void* a1)
  {
    ORIGINAL(loading_world_new_game_loop)(a1);
    // offset of stage may changed!
    auto state = (int*)((uintptr_t)a1 + 292);
    if (*state == 1) {
      StateManager::GetSingleton()->State(StateManager::Loading);
    }
    if (*state > 1 && *state <= 29) {
      StateManager::GetSingleton()->State(StateManager::Game);
    }
  }

  // loading_world_continuing_game_loop interface loop
  // Loading world and continuing active game
  SETUP_ORIG_FUNC(loading_world_continuing_game_loop);
  void __fastcall HOOK(loading_world_continuing_game_loop)(__int64 a1)
  {
    ORIGINAL(loading_world_continuing_game_loop)(a1);
    // offset of stage may changed!
    auto state = (int*)((uintptr_t)a1 + 32);
    if (*state > 0 && *state <= 2) {
      StateManager::GetSingleton()->State(StateManager::Loading);
    }
    if (*state > 2 && *state < 50) {
      StateManager::GetSingleton()->State(StateManager::Game);
    }
  }

  // loading_world_start_new_game_loop interface loop
  // Loading world to start new game
  SETUP_ORIG_FUNC(loading_world_start_new_game_loop);
  void __fastcall HOOK(loading_world_start_new_game_loop)(__int64 a1)
  {
    ORIGINAL(loading_world_start_new_game_loop)(a1);
    // offset of stage may changed!
    auto state = (int*)((uintptr_t)a1 + 360);
    if (*state > 0 && *state <= 4) {
      StateManager::GetSingleton()->State(StateManager::Loading);
    }
    if (*state > 3 && *state < 34) {
      StateManager::GetSingleton()->State(StateManager::Game);
    }
  }

  // menu_interface_loop main menu interface loop
  SETUP_ORIG_FUNC(menu_interface_loop);
  void __fastcall HOOK(menu_interface_loop)(__int64 a1)
  {
    ORIGINAL(menu_interface_loop)(a1);
    StateManager::GetSingleton()->State(StateManager::Menu);
  }

  // search section below
  void Capitalize(char& s)
  {
    // latin capitalize
    if (s >= 'a' && s <= 'z') {
      s -= 'a';
      s += 'A';
    }
    // cyrillic capitalize
    if (s >= char(0xE0) && s <= char(0xFF)) {
      s -= char(0xE0);
      s += char(0xC0);
    }
    // cyrillic ё
    if (s == char(0xB8)) {
      s = char(0xA8);
    }
  }

  void LowerCast(char& s)
  {
    // latin lowercast
    if (s >= 'A' && s <= 'Z') {
      s -= 'A';
      s += 'a';
    }
    // cyrillic lowercast
    if (s >= char(0xC0) && s <= char(0xDF)) {
      s -= char(0xC0);
      s += char(0xE0);
    }
    // cyrillic ё
    if (s == char(0xA8)) {
      s = char(0xB8);
    }
  }

  // main handler for input from keyboard
  SETUP_ORIG_FUNC(standardstringentry);
  int __fastcall HOOK(standardstringentry)(std::string& str, int maxlen, unsigned int flag,
                                           std::set<InterfaceKey>& events)
  {
    char entry = char(1);

    if (flag & STRINGENTRY_SYMBOLS) {
      for (short int item = INTERFACEKEY_STRING_A000; item <= INTERFACEKEY_STRING_A255; item++) {
        if (events.count(item)) {
          entry = char(item - 357);
          break;
        }
      }
    }
    if (flag & STRINGENTRY_LETTERS) {
      // latin capitals
      for (short int item = INTERFACEKEY_STRING_A065; item <= INTERFACEKEY_STRING_A090; item++) {
        if (events.count(item)) {
          entry = char(item - 357);
          break;
        }
      }
      // latin small
      for (short int item = INTERFACEKEY_STRING_A097; item <= INTERFACEKEY_STRING_A122; item++) {
        if (events.count(item)) {
          entry = char(item - 357);
          break;
        }
      }
      // cyrillic
      for (short int item = INTERFACEKEY_STRING_A192; item <= INTERFACEKEY_STRING_A255; item++) {
        if (events.count(item)) {
          entry = char(item - 356);
          break;
        }
      }
    }
    if (flag & STRINGENTRY_SPACE) {
      if (events.count(INTERFACEKEY_STRING_A032)) {
        entry = ' ';
      }
    }
    if (events.count(INTERFACEKEY_STRING_A000)) {
      entry = char(0);
    }
    if (flag & STRINGENTRY_NUMBERS) {
      // numbers
      for (short int item = INTERFACEKEY_STRING_A048; item <= INTERFACEKEY_STRING_A057; item++) {
        if (events.count(item)) {
          entry = char(item - 357);
          break;
        }
      }
    }

    if (entry != 1) {
      if (entry == 0) {
        if (str.size() > 0) {
          str.resize(str.size() - 1);
        }
      } else {
        int cursor = str.size();
        if (cursor >= maxlen) {
          cursor = maxlen - 1;
        }
        if (cursor < 0) {
          cursor = 0;
        }
        if (str.size() < cursor + 1) {
          str.resize(cursor + 1);
        }
        if (flag & STRINGENTRY_CAPS) {
          Capitalize(entry);
        }

        str[cursor] = entry;
      }
      events.clear();
      return 1;
    }

    return 0;
    // return ORIGINAL(standardstringentry)(str, maxlen, flag, events);
  }

  SETUP_ORIG_FUNC(simplify_string);
  void __fastcall HOOK(simplify_string)(std::string& str)
  {
    for (int s = 0; s < str.size(); s++) {
      LowerCast(str[s]);
      switch (str[s]) {
        case (char)129:
        case (char)150:
        case (char)151:
        case (char)154:
        case (char)163:
          str[s] = 'u';
          break;
        case (char)152:
          str[s] = 'y';
          break;
        case (char)164:
        case (char)165:
          str[s] = 'n';
          break;
        case (char)131:
        case (char)132:
        case (char)133:
        case (char)134:
        case (char)142:
        case (char)143:
        case (char)145:
        case (char)146:
        case (char)160:
          str[s] = 'a';
          break;
        case (char)130:
        case (char)136:
        case (char)137:
        case (char)138:
        case (char)144:
          str[s] = 'e';
          break;
        case (char)139:
        case (char)140:
        case (char)141:
        case (char)161:
          str[s] = 'i';
          break;
        case (char)147:
        case (char)148:
        case (char)149:
        case (char)153:
        case (char)162:
          str[s] = 'o';
          break;
        case (char)128:
        case (char)135:
          str[s] = 'c';
          break;
      }
    }
  }

  SETUP_ORIG_FUNC(upper_case_string);
  void __fastcall HOOK(upper_case_string)(std::string& str)
  {
    for (int s = 0; s < str.size(); s++) {
      Capitalize(str[s]);
      switch (str[s]) {
        case (char)129:
          str[s] = (char)154;
          break;
        case (char)164:
          str[s] = (char)165;
          break;
        case (char)132:
          str[s] = (char)142;
          break;
        case (char)134:
          str[s] = (char)143;
          break;
        case (char)130:
          str[s] = (char)144;
          break;
        case (char)148:
          str[s] = (char)153;
          break;
        case (char)135:
          str[s] = (char)128;
          break;
        case (char)145:
          str[s] = (char)146;
          break;
      }
    }
  }

  SETUP_ORIG_FUNC(lower_case_string);
  void __fastcall HOOK(lower_case_string)(std::string& str)
  {
    for (int s = 0; s < str.size(); s++) {
      LowerCast(str[s]);
      switch (str[s]) {
        case (char)154:
          str[s] = (char)129;
          break;
        case (char)165:
          str[s] = (char)164;
          break;
        case (char)142:
          str[s] = (char)132;
          break;
        case (char)143:
          str[s] = (char)134;
          break;
        case (char)144:
          str[s] = (char)130;
          break;
        case (char)153:
          str[s] = (char)148;
          break;
        case (char)128:
          str[s] = (char)135;
          break;
        case (char)146:
          str[s] = (char)145;
          break;
      }
    }
  }

  SETUP_ORIG_FUNC(capitalize_string_words);
  void __fastcall HOOK(capitalize_string_words)(std::string& str)
  {
    for (int s = 0; s < str.size(); s++) {
      char conf = 0;
      if (s > 0) {
        if (str[s - 1] == ' ' || str[s - 1] == '\"')
          conf = 1;
        if (str[s - 1] == '\'') {
          if (s <= 0)
            conf = 1;
          else if (s >= 2) {
            if (str[s - 2] == ' ' || str[s - 2] == ',')
              conf = 1;
          }
        }
      }
      if (s == 0 || conf) {
        Capitalize(str[s]);
        switch (str[s]) {
          case (char)129:
            str[s] = (char)154;
            break;
          case (char)164:
            str[s] = (char)165;
            break;
          case (char)132:
            str[s] = (char)142;
            break;
          case (char)134:
            str[s] = (char)143;
            break;
          case (char)130:
            str[s] = (char)144;
            break;
          case (char)148:
            str[s] = (char)153;
            break;
          case (char)135:
            str[s] = (char)128;
            break;
          case (char)145:
            str[s] = (char)146;
            break;
        }
      }
    }
  }

  SETUP_ORIG_FUNC(capitalize_string_first_word);
  void __fastcall HOOK(capitalize_string_first_word)(std::string& str)
  {
    for (int s = 0; s < str.size(); s++) {
      char conf = 0;
      if (s > 0) {
        if (str[s - 1] == ' ' || str[s - 1] == '\"')
          conf = 1;
        if (str[s - 1] == '\'') {
          if (s <= 0)
            conf = 1;
          else if (s >= 2) {
            if (str[s - 2] == ' ' || str[s - 2] == ',')
              conf = 1;
          }
        }
      }
      if (s == 0 || conf) {
        Capitalize(str[s]);
        switch (str[s]) {
          case (char)129:
            str[s] = (char)154;
            return;
          case (char)164:
            str[s] = (char)165;
            return;
          case (char)132:
            str[s] = (char)142;
            return;
          case (char)134:
            str[s] = (char)143;
            return;
          case (char)130:
            str[s] = (char)144;
            return;
          case (char)148:
            str[s] = (char)153;
            return;
          case (char)135:
            str[s] = (char)128;
            return;
          case (char)145:
            str[s] = (char)146;
            return;
        }
        if (str[s] != ' ' && str[s] != '\"')
          return;
      }
    }
  }

  // init TTFManager
  // should call init() for TTFInit and SDL function load from dll
  // then should load font for drawing text
  void InstallTTFInjection()
  {
    // auto ttf = TTFManager::GetSingleton();
    // ttf->Init();
    // ttf->LoadFont("terminus_bold.ttf", 14, 2);

    // ttf inject, we swap get every char and swap it to our texture
    ATTACH(addchar);
    ATTACH(addchar_top);
    // ATTACH(add_texture);
    ATTACH(screen_to_texid);
    ATTACH(screen_to_texid_top);
    ATTACH(gps_allocate);
    ATTACH(cleanup_arrays);
  }

  void UninstallTTFInjection()
  {
    DETACH(addchar);
    DETACH(addchar_top);
    // DETACH(add_texture);
    DETACH(screen_to_texid);
    DETACH(screen_to_texid_top);
    DETACH(gps_allocate);
    DETACH(cleanup_arrays);
  }

  // init StateManager, set callback to reset textures cache;
  // StateManager used for tracking game state in case of ttf usage (resetting cache)
  void InstallStateManager()
  {
    auto state = StateManager::GetSingleton();
    state->SetCallback(StateManager::Menu, [&](void) { logger::debug("game state changed to StateManager::Menu"); });
    state->SetCallback(StateManager::Loading, [&](void) {
      // TTFManager::GetSingleton()->ClearCache();
      texture_id_cache.Clear();
      logger::debug("game state changed to StateManager::Loading, clearing texture cache");
    });
    state->SetCallback(StateManager::Game, [&](void) {
      // TTFManager::GetSingleton()->ClearCache();
      texture_id_cache.Clear();
      logger::debug("game state changed to StateManager::Game, clearing texture cache");
    });

    // game state tracking
    ATTACH(loading_world_new_game_loop);
    ATTACH(loading_world_continuing_game_loop);
    ATTACH(loading_world_start_new_game_loop);
    ATTACH(menu_interface_loop);
  }

  void UninstallStateManager()
  {
    DETACH(loading_world_new_game_loop);
    DETACH(loading_world_continuing_game_loop);
    DETACH(loading_world_start_new_game_loop);
    DETACH(menu_interface_loop);
  }

  void InstallTranslation()
  {
    // translation
    // ATTACH(string_copy);
    ATTACH(string_copy_n);
    ATTACH(string_append);
    ATTACH(string_append_0);
    ATTACH(string_append_n);
    ATTACH(convert_ulong_to_string);
    ATTACH(addst);
    ATTACH(addst_top);
    ATTACH(addst_flag);
    ATTACH(addcoloredst);
    // ATTACH(addst_template);

    // search handling
    if (Config::Setting::enable_search) {
      ATTACH(standardstringentry);
      ATTACH(simplify_string);
      ATTACH(upper_case_string);
      ATTACH(lower_case_string);
      ATTACH(capitalize_string_words);
      ATTACH(capitalize_string_first_word);
    }
  }

  void UninstallTranslation()
  {
    // translation
    // DETACH(string_copy);
    DETACH(string_copy_n);
    DETACH(string_append);
    DETACH(string_append_0);
    DETACH(string_append_n);
    DETACH(convert_ulong_to_string);
    DETACH(addst);
    DETACH(addst_top);
    DETACH(addst_flag);
    DETACH(addcoloredst);
    // DETACH(addst_template);

    // search handling
    if (Config::Setting::enable_search) {
      DETACH(standardstringentry);
      DETACH(simplify_string);
      DETACH(upper_case_string);
      DETACH(lower_case_string);
      DETACH(capitalize_string_words);
      DETACH(capitalize_string_first_word);
    }
  }

}