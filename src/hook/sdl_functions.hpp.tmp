namespace SDL {

  typedef SDL_Surface* (*SDL_DisplayFormat_)(void* buf);
  typedef SDL_Surface* (*SDL_CreateRGBSurface_)(Uint32 flags, int width, int height, int depth, Uint32 Rmask,
                                                Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
  typedef Uint32 (*SDL_MapRGB_)(const SDL_PixelFormat* const format, const Uint8 r, const Uint8 g, const Uint8 b);
  typedef void (*SDL_FreeSurface_)(SDL_Surface* surface);
  typedef int (*SDL_FillRect_)(SDL_Surface* dst, SDL_Rect* dstrect, Uint32 color);
  typedef int (*SDL_UpperBlit_)(SDL_Surface* src, SDL_Rect* srcrect, SDL_Surface* dst, SDL_Rect* dstrect);
  typedef char* (*SDL_GetError_)(void);
  typedef SDL_Surface* (*SDL_GetVideoSurface_)(void);

  SDL_DisplayFormat_ DisplayFormat;
  SDL_CreateRGBSurface_ CreateRGBSurface;
  SDL_MapRGB_ MapRGB;
  SDL_FreeSurface_ FreeSurface;
  SDL_FillRect_ FillRect;
  SDL_UpperBlit_ UpperBlit;
  SDL_GetError_ GetError;
  SDL_GetVideoSurface_ GetVideoSurface;

  void LoadSDLAdresses()
  {

    HMODULE hModule = LoadLibrary(TEXT("SDL.DLL"));
    if (!hModule) {
      logger::error("unable to load SDL.dll");
      exit(2);
    }

    // instant crash on call, why?
    DisplayFormat = (SDL_DisplayFormat_)GetProcAddress(hModule, "SDL_DisplayFormat");
    CreateRGBSurface = (SDL_CreateRGBSurface_)GetProcAddress(hModule, "SDL_CreateRGBSurface");
    MapRGB = (SDL_MapRGB_)GetProcAddress(hModule, "SDL_MapRGB");
    FreeSurface = (SDL_FreeSurface_)GetProcAddress(hModule, "SDL_FreeSurface");
    FillRect = (SDL_FillRect_)GetProcAddress(hModule, "SDL_FillRect");
    UpperBlit = (SDL_UpperBlit_)GetProcAddress(hModule, "SDL_UpperBlit");
    GetError = (SDL_GetError_)GetProcAddress(hModule, "SDL_GetError");
    GetVideoSurface = (SDL_GetVideoSurface_)GetProcAddress(hModule, "SDL_GetVideoSurface");
  }

}