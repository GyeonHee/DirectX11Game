#pragma once
#include "Core.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <memory>
#include <string>

class Engine_API Engine
{
public:
	Engine(HINSTANCE hInstance, const std::wstring& title, UINT width, UINT height);
	virtual ~Engine();

	static Engine& Get();

	static LRESULT CALLBACK MessageProcedure(HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);

	void Run();

	//void SetLevel(std::shared_ptr<class Level> newLevel);

	void OnResize(UINT width, UINT height);

	void Quit();

	// Getter.
	ID3D11Device& Device() const;
	ID3D11DeviceContext& Context() const;
	class Renderer& GetRenderer() const;

	ImGuiContext* GetImGuiContext() const { return imguiContext; }

	UINT Width() const;
	UINT Height() const;

	virtual void OnGUI() {}

protected:
	static Engine* instance;

	bool isQuit = false;

	std::shared_ptr<class Window> window;

	std::shared_ptr<class Renderer> renderer;

	//std::unique_ptr<class Input> input;

	//std::shared_ptr<class Level> mainLevel;
	
	ImGuiContext* imguiContext = nullptr;
};