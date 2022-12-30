#include "screen_manager.h"

void ScreenManager::AllocateScreen(int x, int y)
{
  this->dimx = x;
  this->dimy = y;
  this->screen = new ScreenTile[x * y * 4];
}

void ScreenManager::ClearScreen()
{
  if (this->screen != nullptr) {
    delete[] this->screen;
  }
  this->screen = nullptr;
}

ScreenTile* ScreenManager::GetTile(int x, int y)
{
  return this->screen + x * this->dimy * 4 + y * 4;
}

ScreenTile* ScreenManager::GetOffset(int offset)
{
  return this->screen + offset * 4;
}

bool ScreenManager::isInitialized()
{
  return this->screen != nullptr;
}