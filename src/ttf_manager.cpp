#include "ttf_manager.h"
#include "sdl_functions.hpp"

// @raydan scaling methods
// TODO: rework it to look more prettier

Uint32 ReadPixel(SDL_Surface* surface, int x, int y)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

  switch (bpp) {
    case 1:
      return *p;
      break;

    case 2:
      return *(Uint16*)p;
      break;

    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
        return p[0] << 16 | p[1] << 8 | p[2];
      else
        return p[0] | p[1] << 8 | p[2] << 16;
      break;

    case 4:
      return *(Uint32*)p;
      break;

    default:
      return 0; /* shouldn't happen, but avoids warnings */
  }
}

void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to set */
  Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

  switch (bpp) {
    case 1:
      *p = pixel;
      break;

    case 2:
      *(Uint16*)p = pixel;
      break;

    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
        p[0] = (pixel >> 16) & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = pixel & 0xff;
      } else {
        p[0] = pixel & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = (pixel >> 16) & 0xff;
      }
      break;

    case 4:
      *(Uint32*)p = pixel;
      break;
  }
}

SDL_Surface* ScaleSurface(SDL_Surface* Surface, Uint16 Width, Uint16 Height)
{
  if (!Surface || !Width || !Height)
    return 0;

  SDL_Surface* _ret =
    SDL_CreateRGBSurface_ptr(Surface->flags, Width, Height, Surface->format->BitsPerPixel, Surface->format->Rmask,
                             Surface->format->Gmask, Surface->format->Bmask, Surface->format->Amask);

  double _stretch_factor_x = (static_cast<double>(Width) / static_cast<double>(Surface->w)),
         _stretch_factor_y = (static_cast<double>(Height) / static_cast<double>(Surface->h));

  for (Sint32 y = 0; y < Surface->h; y++)
    for (Sint32 x = 0; x < Surface->w; x++)
      for (Sint32 o_y = 0; o_y < _stretch_factor_y; ++o_y)
        for (Sint32 o_x = 0; o_x < _stretch_factor_x; ++o_x)
          DrawPixel(_ret, static_cast<Sint32>(_stretch_factor_x * x) + o_x,
                    static_cast<Sint32>(_stretch_factor_y * y) + o_y, ReadPixel(Surface, x, y));

  return _ret;
}

void TTFManager::Init()
{
  if (TTF_Init() == -1) {
    spdlog::error("SDL_ttf init error");
    exit(2);
  }
  LoadSDLAdresses();
  // seems like game deleting surface
  // this->cache.SetEraseCallback([](const std::string& key, SDL_Surface* value) { SDL_FreeSurface_ptr(value); });
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

// TODO: should be properly scaled, add more options for scaling methods
SDL_Surface* TTFManager::CreateTexture(const std::string& str, SDL_Color font_color)
{
  // checkout cache and return if exist
  auto cached = this->cache.Get(str);
  if (cached) {
    return cached.value().get();
  }
  if (this->font == nullptr) {
    spdlog::error("Trying create texture before setting font");
    exit(2);
  }
  // create texture
  auto texture = TTF_RenderUTF8_Blended(this->font, str.c_str(), font_color);
  // scale to target tile size
  texture = ScaleSurface(texture, 8, 12);

  // TODO: remove cached response flag
  if (this->cached_response) {
    this->cache.Put(str, texture);
  }
  return texture;
}

// void TTFManager::DrawString(const std::string& str, int x, int y, int width, int height, Justify justify,
//                             SDL_Surface* screen)
// {
//   auto string_texture = this->CreateTexture(str);

//   if (this->screen == nullptr) {
//     this->LoadScreen();
//     if (this->screen == nullptr) {
//       spdlog::error("Unable to get VideoSurface");
//       exit(2);
//     }
//   }

//   SDL_Rect dest;
//   dest.x = x;
//   dest.y = y + string_texture->height - height;

//   if (justify == Justify::CENTER) {
//     dest.x = x + (width - string_texture->width) / 2;
//   }

//   // spdlog::debug("str {},w {},  h {}, tex w {}, tex h {},", str, width, height, string_texture.width,
//   //               string_texture.height);
//   SDL_UpperBlit_ptr(string_texture->texture, NULL, screen != nullptr ? screen : this->screen, &dest);
//   SDL_FreeSurface_ptr(string_texture->texture);
// }

void TTFManager::ClearCache()
{
  this->cache.Clear();
}
