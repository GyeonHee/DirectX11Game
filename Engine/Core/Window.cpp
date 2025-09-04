#include "Window.h"

Window::Window(
	HINSTANCE instance,
	WNDPROC messageProcedure,
	const std::wstring& title,
	UINT width,
	UINT height)
	: width(width), height(height), title(title), instance(instance)
{

	WNDCLASS wc = { };
	wc.lpfnWndProc = messageProcedure;
	wc.hInstance = instance;		
	wc.lpszClassName = className.c_str();

	if (!RegisterClass(&wc))
	{
		OutputDebugStringA("Failed to register a window class\n");

		MessageBoxA(nullptr, "Failed to register a window", "Error", MB_OK);

		__debugbreak();
	}

	unsigned int xPosition = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	unsigned int yPosition = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;


	RECT rect{ 0, 0, (long)width, (long)height };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);


	unsigned int windowWidth = rect.right - rect.left;
	unsigned int windowHeight = rect.bottom = rect.bottom;


	handle = CreateWindow(
		className.c_str(),      // Window class
		title.c_str(),          // Window text
		WS_OVERLAPPEDWINDOW,    // Window style
		// Size and position
		xPosition, yPosition,
		windowWidth, windowHeight,
		nullptr,       // Parent window
		nullptr,       // Menu
		instance,  // Instance handle
		nullptr
	);

	if (!handle)
	{
		OutputDebugStringA("Failed to create a window class\n");

		MessageBoxA(nullptr, "Failed to create a window", "Error", MB_OK);

		__debugbreak();
	}

	ShowWindow(handle, SW_SHOW);
	UpdateWindow(handle);
}

Window::~Window()
{
	// 등록 해제
	UnregisterClass(className.c_str(), instance);
}

void Window::SetWidthHeight(UINT width, UINT height)
{
	this->width = width;
	this->height = height;
}