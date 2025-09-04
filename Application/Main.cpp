#include <iostream>

#include <Windows.h>
#include "Game/Game.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define WINDOW_NAME TEXT("DirectX_11")


//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

//-----------------------------------------------------------------------------
// Name: wWinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
int APIENTRY wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nCmdShow)
{
	Game game(GetModuleHandle(nullptr), WINDOW_NAME, SCREEN_WIDTH, SCREEN_HEIGHT);
	game.Run();
	return 0;
}