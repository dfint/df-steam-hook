#include "ttf_manager.h"

#include "sdl_functions.hpp"
#include "utils.hpp"

// @raydan scaling methods
// TODO: rework it to look more prettier

Uint32 TTFManager::ReadPixel(SDL_Surface* surface, int x, int y)
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

void TTFManager::DrawPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
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

SDL_Surface* TTFManager::ScaleSurface(SDL_Surface* Surface, Uint16 Width, Uint16 Height)
{
   if (!Surface || !Width || !Height) return 0;

   SDL_Surface* _ret = SDL::CreateRGBSurface(Surface->flags, Width, Height, Surface->format->BitsPerPixel, Surface->format->Rmask,
                                             Surface->format->Gmask, Surface->format->Bmask, Surface->format->Amask);

   double _stretch_factor_x = (static_cast<double>(Width) / static_cast<double>(Surface->w)),
          _stretch_factor_y = (static_cast<double>(Height) / static_cast<double>(Surface->h));

   for (Sint32 y = 0; y < Surface->h; y++)
      for (Sint32 x = 0; x < Surface->w; x++)
         for (Sint32 o_y = 0; o_y < _stretch_factor_y; ++o_y)
            for (Sint32 o_x = 0; o_x < _stretch_factor_x; ++o_x)
               DrawPixel(_ret, static_cast<Sint32>(_stretch_factor_x * x) + o_x, static_cast<Sint32>(_stretch_factor_y * y) + o_y,
                         ReadPixel(Surface, x, y));

   return _ret;
}

SDL_Surface* TTFManager::SliceSurface(SDL_Surface* surface, int slicex, Uint16 width, Uint16 height, int shift_frame_from_up)
{
   if (!surface || !width || !height) return 0;

   SDL_Surface* sliced_tex = SDL::CreateRGBSurface(surface->flags, width, height, surface->format->BitsPerPixel, surface->format->Rmask,
                                                   surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

   int sx = 0;
   for (int y = shift_frame_from_up; y < surface->h; y++) {
      for (int x = slicex; x < surface->w; x++) {
         if (sx < width && y - shift_frame_from_up < height) {
            DrawPixel(sliced_tex, sx, y - shift_frame_from_up, ReadPixel(surface, x, y));
         }
         sx++;
      }
      sx = 0;
   }

   return sliced_tex;
}

SDL_Surface* TTFManager::ResizeSurface(SDL_Surface* surface, int width, int height, int shift_frame)
{
   // should check cause it may crashed
   if (!surface || !width || !height) {
      return 0;
   }

   SDL_Surface* sized_texture = SDL::CreateRGBSurface(surface->flags, width, height, surface->format->BitsPerPixel, surface->format->Rmask,
                                                      surface->format->Gmask, surface->format->Bmask, surface->format->Amask);

   if (shift_frame < 0) {  // Shift Down
      shift_frame *= -1;
      for (int y = shift_frame; y < surface->h; y++) {
         for (int x = 0; x < surface->w; x++) {
            if (x < width && y - shift_frame < height) {
               DrawPixel(sized_texture, x, y, ReadPixel(surface, x, y - shift_frame));
            }
         }
      }
   } else {  // Shift Up
      for (int y = shift_frame; y < surface->h; y++) {
         for (int x = 0; x < surface->w; x++) {
            if (x < width && y - shift_frame < height) {
               DrawPixel(sized_texture, x, y - shift_frame, ReadPixel(surface, x, y));
            }
         }
      }
   }

   // clear source surface
   SDL::FreeSurface(surface);

   return sized_texture;
}

void TTFManager::Init()
{
   if (TTF_Init() == -1) {
      logger::error("SDL_ttf init error");
      exit(2);
   }
   SDL::LoadSDLAdresses();
   // seems like game deleting surface
   // this->cache.SetEraseCallback([](const std::string& key, SDL_Surface* value) { SDL_FreeSurface_ptr(value); });
   logger::info("TTFManager loaded");
}

void TTFManager::ClearCache()
{
   this->cache.Clear();
   this->ws_cache.Clear();
   this->sliced_ws_cache.Clear();
}

void TTFManager::LoadFont(const std::string& file, int ptsize, int shift_frame_from_up)
{
   if (this->font != nullptr) {
      TTF_CloseFont(this->font);
   }
   auto font = TTF_OpenFont(file.c_str(), ptsize);
   this->font = font;
   this->shift_frame_from_up = shift_frame_from_up;
   logger::debug("load font 0x{:x}", (uintptr_t)this->font);
}

void TTFManager::LoadScreen()
{
   auto screen = SDL::GetVideoSurface();
   this->screen = screen;
   logger::debug("load screen 0x{:x}", (uintptr_t)this->screen);
}

SDL_Surface* TTFManager::GetSlicedTexture(const std::wstring& wstr)
{
   auto cached = this->sliced_ws_cache.Get(wstr);
   if (cached) {
      return cached.value().get();
   }
   spdlog::error("some sliced texture error :{}", Utils::ws2s(wstr));
   return nullptr;
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
      logger::error("trying to create texture before setting font");
      exit(2);
   }

   auto texture = TTF_RenderUTF8_Blended(this->font, str.c_str(), font_color);
   // TODO: rework missing utf8 chars  and font glyphs
   if (texture == NULL) {
      logger::error("texture generation error on string {}", str);
      texture = TTF_RenderUTF8_Blended(this->font, "x", font_color);
   }
   // scale to target tile size
   texture = ResizeSurface(texture, TTFManager::frame_width, TTFManager::frame_height, this->shift_frame_from_up);
   this->cache.Put(str, texture);

   return texture;
}

// 문자열을 받아서 텍스쳐 생성 후 타일 너비에 맞게 자른 후 캐시에 저장
int TTFManager::CreateWSTexture(const std::wstring& wstr, int flag, SDL_Color font_color)
{
   if (this->font == nullptr) {
      spdlog::error("trying to create texture before setting font");
      exit(2);
   }
   int count = 0;
   std::wstring input(wstr);
   if (flag > 0) input += std::to_wstring(flag);
   // 문자열 텍스쳐 캐시 확인
   auto cached = this->ws_cache.Get(input);
   if (cached) {
      auto cached_tex = cached.value().get();
      // 문자열 텍스쳐 타일너비로 개수 계산
      count = cached_tex->w / this->frame_width + (cached_tex->w % this->frame_width != 0);
      return count;
   }

   // 문자열 텍스쳐 생성
   // std::wstring wstr = Utils::s2ws(str);
   auto texture = TTF_RenderUNICODE_Blended(this->font, (Uint16*)wstr.c_str(), font_color);
   if (texture == NULL) {
      spdlog::error("texture generation error on string");
      return false;
   }

   int flag_shift = 0;
   if (flag == 8) {  // Shift down
      texture = ResizeSurface(texture, texture->w, texture->h, std::round(texture->h * -0.45));
      flag_shift = this->shift_frame_from_up - 1;
   }

   if (flag == 16) {  // Shift up
      texture = ResizeSurface(texture, texture->w, texture->h, std::round(texture->h * 0.55));
      flag_shift = this->shift_frame_from_up * -1;
   }
   this->ws_cache.Put(input, texture);

   // 문자열 텍스쳐 자르기
   count = texture->w / this->frame_width + (texture->w % this->frame_width != 0);
   for (int x = 0; x < count; x++) {
      auto slice_texture = SliceSurface(texture, x * this->frame_width, this->frame_height, this->frame_height, this->shift_frame_from_up + flag_shift);
      std::wstring temp(input);
      temp += std::to_wstring(x);
      if(flag > 0) spdlog::debug("#### Input slice texture {}",Utils::ws2s(temp));
      this->sliced_ws_cache.Put(temp, slice_texture);
   }

   return count;
}
