#include "cache.hpp"

class TTFManager
{
public:
  [[nodiscard]] static TTFManager* GetSingleton()
  {
    static TTFManager singleton;
    return &singleton;
  }

  void Init();
  void LoadFont(const std::string& file, int ptsize, int shift_frame_from_up = 0);
  void LoadScreen();
  SDL_Surface* CreateTexture(const std::string& str, SDL_Color font_color = { 255, 255, 255 });
  void ClearCache();

private:
  TTFManager() {}
  TTFManager(const TTFManager&) = delete;
  TTFManager(TTFManager&&) = delete;

  ~TTFManager()
  {
    if (this->font != nullptr) {
      TTF_CloseFont(font);
    }
    delete this;
  };

  SDL_Surface* ResizeSurface(SDL_Surface* surface, int width, int height, int shift_frame_from_up);
  SDL_Surface* ScaleSurface(SDL_Surface* Surface, Uint16 Width, Uint16 Height);
  void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
  Uint32 ReadPixel(SDL_Surface* surface, int x, int y);

  // try to figure out the best size of cache
  LRUCache<std::string, SDL_Surface*> cache = LRUCache<std::string, SDL_Surface*>(300);
  TTF_Font* font;
  SDL_Surface* screen;
  int shift_frame_from_up = 0;

  static const int frame_width = 8;
  static const int frame_height = 12;
};