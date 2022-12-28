class TTFManager
{
public:
  [[nodiscard]] static TTFManager* GetSingleton()
  {
    static TTFManager singleton;
    return &singleton;
  }

  void Init();
  void LoadFont(const std::string& file, int ptsize);
  void LoadScreen();
  SDL_Surface* CreateTexture(const std::string& str, SDL_Color font_color);
  void DrawString(const std::string& str, int x, int y);

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

  TTF_Font* font;
  SDL_Surface* screen;
};