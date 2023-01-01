class ScreenManager
{
public:
  [[nodiscard]] static ScreenManager* GetSingleton()
  {
    static ScreenManager singleton;
    return &singleton;
  }

  struct ScreenTile
  {
    long unk;
    long tex_pos;
    long unk2;
    long unk3;
  };

  ScreenTile* GetTile(int x, int y) { return this->screen + x * this->dimy * 4 + y * 4; }
  ScreenTile* GetOffset(int offset) { return this->screen + offset * 4; }
  void AllocateScreen(int x, int y)
  {
    this->dimx = x;
    this->dimy = y;
    this->screen = new ScreenTile[x * y * 4];
  }
  void ClearScreen()
  {
    if (this->screen != nullptr) {
      delete[] this->screen;
    }
    this->screen = nullptr;
  }
  bool isInitialized() { return this->screen != nullptr; }

private:
  ScreenManager() {}
  ScreenManager(const ScreenManager&) = delete;
  ScreenManager(ScreenManager&&) = delete;

  ~ScreenManager()
  {
    delete[] this->screen;
    delete this;
  };

  int32_t dimx;
  int32_t dimy;
  ScreenTile* screen;
};