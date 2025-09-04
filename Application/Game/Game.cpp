#include "Game.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <string>

Game::Game(HINSTANCE hInstance, const std::wstring& title, UINT width, UINT height) : Engine(hInstance, title, width, height)
{
}

Game::~Game()
{
}

void Game::OnGUI()
{

}
