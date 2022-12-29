#include "screen_manager.h"

void ScreenManager::AllocateScreen(int x, int y)
{
  this->screen = new unsigned long[x * y * 12];
}

void ScreenManager::ClearScreen()
{
  if (this->screen != nullptr) {
    delete[] this->screen;
  }
  this->screen = nullptr;
}

// found way to store dimy in instanse? maybe on allocate?
ScreenTile* ScreenManager::GetTile(int x, int y, int dimy)
{
  auto s = this->screen + x * dimy * 4 + y * 4;
  return reinterpret_cast<ScreenTile*>(s);
}

ScreenTile* ScreenManager::GetOffset(int offset)
{
  auto s = this->screen + offset * 4;
  return reinterpret_cast<ScreenTile*>(s);
}

bool ScreenManager::isInitialized()
{
  return this->screen != nullptr;
}