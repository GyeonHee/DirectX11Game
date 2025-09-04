#pragma once

// Assimp 충돌 방지 매크로들
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN  
#define WIN32_LEAN_AND_MEAN
#endif

// dll ??η? ???ø? ?????? ?? ?????? ??? ??????
#pragma warning(disable: 4251)
// ???? ?????? ???? ????? ?? ?????? ??? ??????
#pragma warning(disable: 4172)

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <windows.h>

// Windows.h 이후 매크로 정리
#ifdef min
#undef min
#endif
#ifdef max
#undef max  
#endif
#include <memory>
#include <string>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#if BuildEngineDLL
#define Engine_API __declspec(dllexport)
#else
#define Engine_API __declspec(dllimport)
#endif

//#ifdef _DEBUG
//#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
//// allocations to be of _CLIENT_BLOCK type
//#else
//#define new new
//#endif

// ??? ???? ???
template<typename T>
void SafeDelete(T*& target)
{
	if (target != nullptr)
	{
		delete target;
		target = nullptr;
	}
}

// ??? ???? ???
template<typename T>
void SafeDeleteArray(T*& target)
{
	if (target != nullptr)
	{
		delete[] target;
		target = nullptr;
	}
}

template<typename T>
void SafeRelease(T*& target)
{
	if (target != nullptr)
	{
		target->Release();
		target = nullptr;
	}
}