#include "Game.h"

#include <string>

Game::Game(HINSTANCE hInstance, const std::wstring& title, UINT width, UINT height) : Engine(hInstance, title, width, height)
{
}

Game::~Game()
{
}