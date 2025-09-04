#pragma once
#include "Engine.h"

class Game : public Engine
{
public:
    Game(HINSTANCE hInstance, const std::wstring& title, UINT width, UINT height);
    ~Game();

    void OnGUI() override;
};