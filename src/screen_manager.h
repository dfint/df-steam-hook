struct ScreenTile
{
  long unk;
  long tex_pos;
  long unk2;
  long unk3;
};

class ScreenManager
{
public:
  [[nodiscard]] static ScreenManager* GetSingleton()
  {
    static ScreenManager singleton;
    return &singleton;
  }

  ScreenTile* GetTile(int x, int y);
  ScreenTile* GetOffset(int offset);
  void AllocateScreen(int x, int y);
  void ClearScreen();
  bool isInitialized();

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