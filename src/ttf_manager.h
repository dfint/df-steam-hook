enum Justify : uint8_t
{
  LEFT,
  CENTER,
  RIGHT,
  CONT,
  NOT_TRUETYPE
};

struct StringTexture
{
  SDL_Surface* texture;
  int width;
  int height;

  StringTexture(SDL_Surface* texture, int width, int height)
    : texture(texture)
    , width(width)
    , height(height)
  {
  }
};

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
  std::shared_ptr<StringTexture> CreateTexture(const std::string& str, SDL_Color font_color = { 255, 255, 255 });
  void DrawString(const std::string& str, int x, int y, int width, int height, Justify justify = Justify::LEFT,
                  SDL_Surface* screen = nullptr);

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