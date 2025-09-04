#pragma once

#include "Engine.h"
#include <Windows.h>
#include <string>

class Engine_API Window
{
	friend class Engine;

public:
	Window(
		HINSTANCE instance,
		WNDPROC messageProcedure,
		const std::wstring& title,
		UINT width,
		UINT height
	);

	~Window();

	// Getter/Setter.
	inline const UINT Width() const { return width; }
	inline const UINT Height() const { return height; }
	inline HWND Handle() const { return handle; }
	std::wstring Title() const { return title; }

	void SetWidthHeight(UINT width, UINT height);

private:
	std::wstring title = nullptr;
	std::wstring className = TEXT("Engine Class");
	UINT width = 0;
	UINT height = 0;
	HWND handle = nullptr;
	HINSTANCE instance = nullptr;
};