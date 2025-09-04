#include "Engine.h"
#include "Window.h"
#include "Render/Renderer.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
Engine* Engine::instance = nullptr;

LRESULT Engine::MessageProcedure(
    HWND window,
    UINT message,
    WPARAM wparam,
    LPARAM lparam)
{
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND
        hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    if (ImGui_ImplWin32_WndProcHandler(window, message, wparam,
        lparam))
        return true;
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_QUIT:
        DestroyWindow(window);
        return 0;

    case WM_KEYDOWN:
        if (wparam == VK_ESCAPE)
        {
            if (MessageBoxA(nullptr, "Quit?", "Quit Program", MB_YESNO) == IDYES)
            {
                DestroyWindow(window);
                return 0;
            }
        }
        return 0;
    }
    return DefWindowProc(window, message, wparam, lparam);
}

Engine::Engine(HINSTANCE hInstance, const std::wstring& title, UINT width, UINT height)
{
    instance = this;

    window = std::make_shared<Window>(hInstance, MessageProcedure, title, width, height);

    renderer = std::make_shared<Renderer>(width, height, window->Handle());

    // DirectX 디바이스가 유효한지 확인 후 ImGui 초기화
    if (renderer->device && renderer->context)
    {
        MessageBoxA(nullptr, "Device and Context Valid", "Debug",
            MB_OK);

        IMGUI_CHECKVERSION();

        ImGuiContext* ctx = ImGui::CreateContext();
        if (ctx == nullptr)
        {
            MessageBoxA(nullptr, "ImGui CreateContext Failed!",
                "Error", MB_OK);
            return;
        }

        ImGuiIO& io = ImGui::GetIO();

        bool win32_init = ImGui_ImplWin32_Init(window->Handle());
        if (!win32_init)
        {
            MessageBoxA(nullptr, "ImGui Win32 Init Failed!", "Error",
                MB_OK);
            return;
        }

        bool dx11_init = ImGui_ImplDX11_Init(renderer->device,
            renderer->context);
        if (!dx11_init)
        {
            MessageBoxA(nullptr, "ImGui DX11 Init Failed!", "Error",
                MB_OK);
            return;
        }

        MessageBoxA(nullptr, "ImGui All Init Success", "Debug",
            MB_OK);
    }
    else
    {
        MessageBoxA(nullptr, "Device or Context is NULL!", "Error",
            MB_OK);
    }
}

Engine::~Engine()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Engine::Run()
{
    LARGE_INTEGER currentTime = {};
    LARGE_INTEGER previousTime = currentTime;
    LARGE_INTEGER frequency = {};

    QueryPerformanceFrequency(&frequency);

    QueryPerformanceCounter(&currentTime);

    float targetFrameTime = 120.0f; 
    float oneFrameTime = 1.0f / targetFrameTime; 

    MSG message = {};
    while (message.message != WM_QUIT)
    {
        if (isQuit)
        {
            break;
        }

        if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);

            DispatchMessage(&message);
        }
        else
        {
            QueryPerformanceCounter(&currentTime);

            float deltaTime =
                static_cast<float>(currentTime.QuadPart - previousTime.QuadPart)
                / static_cast<float>(frequency.QuadPart);

            if (deltaTime >= oneFrameTime)
            {
                wchar_t stat[512] = { };
                swprintf_s(stat, 512, TEXT("[%s] - [DeltaTime: %f] [FPS: %d]"),
                    window->Title().c_str(), deltaTime, (int)ceil(1.0f / deltaTime));
                SetWindowText(window->Handle(), stat);

                /*input->SavePreviousKeyStates();
                input->SavePreviousMouseStates();
                input->ProcessInput();*/

               /* if (mainLevel)
                {
                    static bool isLevelInitialized = false;
                    if (!isLevelInitialized)
                    {
                        mainLevel->OnInit();
                        isLevelInitialized = true;
                    }
                    mainLevel->OnUpdate(deltaTime);
                    renderer->OnRender(mainLevel);
                }*/
                if (ImGui::GetCurrentContext())
                {
                    // 1. 3D 렌더링
                    renderer->OnRender();

                    // 2. ImGui 렌더링
                    ImGui_ImplDX11_NewFrame();
                    ImGui_ImplWin32_NewFrame();
                    ImGui::NewFrame();

                    if (ImGui::Begin("Engine Test Window"))
                    {
                        ImGui::SetWindowSize(ImVec2(300, 200));
                        ImGui::Text("Direct from Engine!");
                        ImGui::Button("Test Button");
                        ImGui::End();
                    }

                    ImGui::Render();
                    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

                    // 3. Present 호출
                    renderer->Present();
                }

                previousTime = currentTime;
            }
        }
    }
}

//void Engine::SetLevel(std::shared_ptr<class Level> newLevel)
//{
//    mainLevel = newLevel;
//}

void Engine::OnResize(UINT width, UINT height)
{
    if (!window)
    {
        return;
    }

    if (!renderer)
    {
        return;
    }

    window->SetWidthHeight(width, height);

    RECT rect;
    GetClientRect(window->Handle(), &rect);

    UINT w = (UINT)(rect.right - rect.left);
    UINT h = (UINT)(rect.bottom - rect.top);

    renderer->OnResize(w, h);
}

void Engine::Quit()
{
    isQuit = true;
}

ID3D11Device& Engine::Device() const
{
    return *renderer->device;
}

ID3D11DeviceContext& Engine::Context() const
{
    return *renderer->context;
}

Renderer& Engine::GetRenderer() const
{
    return *renderer;
}

UINT Engine::Width() const
{
    return window->Width();
}

UINT Engine::Height() const
{
    return window->Height();
}