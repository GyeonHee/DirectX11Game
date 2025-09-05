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
    // Engine에서 컨텍스트 가져와서 설정
    if (auto* ctx = Engine::Get().GetImGuiContext())
    {
        ImGui::SetCurrentContext(ctx);
    }
    if (!ImGui::GetCurrentContext()) return;

    ImGui::Begin("Engine Test Window");
    ImGui::SetWindowSize(ImVec2(300, 200));
    ImGui::Text("Direct from Engine!");
    ImGui::Button("Test Button");
    ImGui::End();
}
