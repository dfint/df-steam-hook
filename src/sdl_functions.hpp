typedef SDL_Surface* (*SDL_DisplayFormat_)(void* buf);
typedef SDL_Surface* (*SDL_CreateRGBSurface_)(Uint32 flags, int width, int height, int depth, Uint32 Rmask,
                                              Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
typedef Uint32 (*SDL_MapRGB_)(const SDL_PixelFormat* const format, const Uint8 r, const Uint8 g, const Uint8 b);
typedef void (*SDL_FreeSurface_)(SDL_Surface* surface);
typedef int (*SDL_FillRect_)(SDL_Surface* dst, SDL_Rect* dstrect, Uint32 color);
typedef int (*SDL_UpperBlit_)(SDL_Surface* src, SDL_Rect* srcrect, SDL_Surface* dst, SDL_Rect* dstrect);
typedef char* (*SDL_GetError_)(void);
typedef SDL_Surface* (*SDL_GetVideoSurface_)(void);

SDL_DisplayFormat_ SDL_DisplayFormat_ptr;
SDL_CreateRGBSurface_ SDL_CreateRGBSurface_ptr;
SDL_MapRGB_ SDL_MapRGB_ptr;
SDL_FreeSurface_ SDL_FreeSurface_ptr;
SDL_FillRect_ SDL_FillRect_ptr;
SDL_UpperBlit_ SDL_UpperBlit_ptr;
SDL_GetError_ SDL_GetError_ptr;
SDL_GetVideoSurface_ SDL_GetVideoSurface_ptr;

void LoadSDLAdresses()
{

  HMODULE hModule = LoadLibrary(TEXT("SDL.DLL"));
  if (!hModule) {
    spdlog::error("Unable to laod SDL.dll");
    exit(2);
  }

  // instant crash on call, why?
  SDL_DisplayFormat_ptr = (SDL_DisplayFormat_)GetProcAddress(hModule, "SDL_DisplayFormat");
  SDL_CreateRGBSurface_ptr = (SDL_CreateRGBSurface_)GetProcAddress(hModule, "SDL_CreateRGBSurface");
  SDL_MapRGB_ptr = (SDL_MapRGB_)GetProcAddress(hModule, "SDL_MapRGB");
  SDL_FreeSurface_ptr = (SDL_FreeSurface_)GetProcAddress(hModule, "SDL_FreeSurface");
  SDL_FillRect_ptr = (SDL_FillRect_)GetProcAddress(hModule, "SDL_FillRect");
  SDL_UpperBlit_ptr = (SDL_UpperBlit_)GetProcAddress(hModule, "SDL_UpperBlit");
  SDL_GetError_ptr = (SDL_GetError_)GetProcAddress(hModule, "SDL_GetError");
  SDL_GetVideoSurface_ptr = (SDL_GetVideoSurface_)GetProcAddress(hModule, "SDL_GetVideoSurface");
}