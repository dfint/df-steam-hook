class ScreenManager
{
public:
  [[nodiscard]] static ScreenManager* GetSingleton()
  {
    static ScreenManager singleton;
    return &singleton;
  }

  // TODO: figure out exact struct
  struct ScreenTile
  {
    long symbol;
    long tex_pos;
    uint8_t unk2;
    uint8_t unk3;
    uint8_t unk4;
    uint8_t unk5;
  };

  enum ScreenType
  {
    Main,
    Top
  };

  template <ScreenType screen_type>
  ScreenTile* GetTile(int x, int y)
  {
    if (screen_type == ScreenType::Top) {
      return reinterpret_cast<ScreenTile*>(this->screen_top + x * this->dimy * 4 + y * 4);
    } else {
      return reinterpret_cast<ScreenTile*>(this->screen + x * this->dimy * 4 + y * 4);
    }
  }

  template <ScreenType screen_type>
  ScreenTile* GetOffset(int offset)
  {
    if (screen_type == ScreenType::Top) {
      return reinterpret_cast<ScreenTile*>(this->screen_top + offset * 4);
    } else {
      return reinterpret_cast<ScreenTile*>(this->screen + offset * 4);
    }
  }

  void AllocateScreen(int x, int y)
  {
    this->dimx = x;
    this->dimy = y;
    this->screen = new unsigned long[x * y * 12];
    this->screen_top = new unsigned long[x * y * 12];
  }

  void ClearScreen()
  {
    if (this->screen != nullptr) {
      delete[] this->screen;
    }
    this->screen = nullptr;
    if (this->screen_top != nullptr) {
      delete[] this->screen_top;
    }
    this->screen_top = nullptr;
  }

  bool isInitialized() { return this->screen != nullptr && this->screen_top != nullptr; }

private:
  ScreenManager() {}
  ScreenManager(const ScreenManager&) = delete;
  ScreenManager(ScreenManager&&) = delete;

  ~ScreenManager()
  {
    delete[] this->screen;
    delete[] this->screen_top;
    delete this;
  };

  int32_t dimx;
  int32_t dimy;
  unsigned long* screen;
  unsigned long* screen_top;
};