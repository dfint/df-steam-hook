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
  spdlog::debug("load font 0x{:x}", (uintptr_t)this->font);
}

void TTFManager::LoadScreen()
{
  auto screen = SDL_GetVideoSurface_ptr();
  this->screen = screen;
  spdlog::debug("load screen 0x{:x}", (uintptr_t)this->screen);
}

std::shared_ptr<StringTexture> TTFManager::CreateTexture(const std::string& str, SDL_Color font_color)
{
  // checkout cache and return if exist
  auto cached = this->cache.Get(str);
  if (cached) {
    return cached.value();
  }

  if (this->font == nullptr) {
    spdlog::error("Trying create texture before setting font");
    exit(2);
  }

  // create texture
  auto texture = TTF_RenderUTF8_Blended(this->font, str.c_str(), font_color);
  int width, height;
  TTF_SizeUTF8(this->font, str.c_str(), &width, &height);

  // create struct and put it to cache
  auto string_texture = std::make_shared<StringTexture>(texture, width, height);
  this->cache.Put(str, string_texture);

  return string_texture;
}

void TTFManager::DrawString(const std::string& str, int x, int y, int width, int height, Justify justify,
                            SDL_Surface* screen)
{
  auto string_texture = this->CreateTexture(str);

  if (this->screen == nullptr) {
    this->LoadScreen();
    if (this->screen == nullptr) {
      spdlog::error("Unable to get VideoSurface");
      exit(2);
    }
  }

  SDL_Rect dest;
  dest.x = x;
  dest.y = y + string_texture->height - height;

  if (justify == Justify::CENTER) {
    dest.x = x + (width - string_texture->width) / 2;
  }

  // spdlog::debug("str {},w {},  h {}, tex w {}, tex h {},", str, width, height, string_texture.width,
  //               string_texture.height);
  SDL_UpperBlit_ptr(string_texture->texture, NULL, screen != nullptr ? screen : this->screen, &dest);
  SDL_FreeSurface_ptr(string_texture->texture);
}