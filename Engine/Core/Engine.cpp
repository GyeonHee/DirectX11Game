#include "Engine.h"
#include "Window.h"
#include "Render/Renderer.h"

Engine* Engine::instance = nullptr;

LRESULT Engine::MessageProcedure(
    HWND window,
    UINT message,
    WPARAM wparam,
    LPARAM lparam)
{
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
}

Engine::~Engine()
{

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

                renderer->OnRender();

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