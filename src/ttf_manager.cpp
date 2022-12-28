#include "ttf_manager.h"
#include "sdl_functions.hpp"

void TTFManager::Init()
{
  if (TTF_Init() == -1) {
    spdlog::error("SDL_ttf init error");
    exit(2);
  }
  LoadSDLAdresses();
  spdlog::info("TTFManager loaded");
}

void TTFManager::LoadFont(const std::string& file, int ptsize)
{
  if (this->font != nullptr) {
    TTF_CloseFont(this->font);
  }
  auto font = TTF_OpenFont(file.c_str(), ptsize);
  this->font = font;
  spdlog::debug("load font {}", (uintptr_t)this->font);
}

void TTFManager::LoadScreen()
{
  auto screen = SDL_GetVideoSurface_ptr();
  this->screen = screen;
  spdlog::debug("load screen {}", (uintptr_t)this->screen);
}

SDL_Surface* TTFManager::CreateTexture(const std::string& str,
                                       SDL_Color font_color = { 255, 255, 255 })
{
  // bad conversation
  // TODO: make encoding utf-8 to UCS-2 (Unicode)
  std::wstring widestr = std::wstring(str.begin(), str.end());
  const wchar_t* widecstr = widestr.c_str();

  if (this->font == nullptr) {
    spdlog::error("Trying create texture before setting font");
    exit(2);
  }
  auto texture = TTF_RenderUNICODE_Blended(
    this->font, reinterpret_cast<const Uint16*>(widecstr), font_color);
  return texture;
}

void TTFManager::DrawString(const std::string& str, int x, int y)
{
  auto texture = this->CreateTexture(str);

  if (this->screen == nullptr) {
    this->LoadScreen();
    if (this->screen == nullptr) {
      spdlog::error("Unable to get VideoSurface");
      exit(2);
    }
  }

  SDL_Rect dest;
  dest.x = x;
  dest.y = y;

  SDL_UpperBlit_ptr(texture, NULL, this->screen, &dest);
  SDL_FreeSurface_ptr(texture);
}