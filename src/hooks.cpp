#include "hooks.h"
#include "ttf_manager.h"

// with ttf this hook unneeded
// not used now
SETUP_ORIG_FUNC(add_texture, 0xE827F0);
long __fastcall HOOK(add_texture)(void* ptr, void* a2)
{
  return ORIGINAL(add_texture)(ptr, a2);
}

// string can be catched here
SETUP_ORIG_FUNC(addst, 0x784C60);
void __fastcall HOOK(addst)(graphicst_* gps, string_* str, unsigned __int8 just,
                            int space)
{
  std::string text;
  if (str->len > 15) {
    text = std::string(str->ptr);
  } else {
    text = std::string(str->buf);
  }

  // spdlog::debug("addst {}, screenx {}, screeny {}, screenf {}, screenb {}",
  //               text, gps->screenx, gps->screeny, gps->screenf,
  //               gps->screenb);

  ORIGINAL(addst)(gps, str, just, space);

  TTFManager::GetSingleton()->DrawString("TEST!!", 50, 50);
  return;
}

// screen size can be catched here
// not used for now
SETUP_ORIG_FUNC(create_screen, 0x5BB540);
bool __fastcall HOOK(create_screen)(__int64 a1, unsigned int width,
                                    unsigned int height)
{
  spdlog::debug("create screen width {}, height {}, ptr 0x{:x}", width, height,
                (uintptr_t)a1);
  return ORIGINAL(create_screen)(a1, width, height);
}

void InstallHooks()
{
  // init TTFManager
  // should call init() for TTFInit and SDL function load from dll
  // then should load font for drawing text
  auto ttf = TTFManager::GetSingleton();
  ttf->Init();
  ttf->LoadFont("terminus.ttf", 24);
  ATTACH(add_texture);
  ATTACH(addst);
  ATTACH(create_screen);
  spdlog::info("hooks installed");
}