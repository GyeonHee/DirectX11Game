#pragma once
#include "RTTI.h"
#include <memory>

class Engine_API Renderer : public RTTI
{
	RTTI_DECLARATIONS(Renderer, RTTI)
		friend class Engine;

public:
	Renderer(UINT width, UINT height, HWND window);
	~Renderer();

	void OnRender();// (std::shared_ptr<class Level> level);

	void OnResize(UINT width, UINT height);

	void CullOn();

	void CullOff();

	void WireframeOn();

	void WireframeOff();

private:
	void EmptyRTVsAndSRVs();

	void Clear(ID3D11RenderTargetView** renderTargetView, float* clearColor, ID3D11DepthStencilView* depthStencilView);

	//void DrawToRenderTexturePass(std::shared_ptr<Level>& level);

	//void DrawFinalPass(std::shared_ptr<Level>& level);

private:
	bool isResizing = false;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGISwapChain* swapChain = nullptr;

	ID3D11RenderTargetView* renderTargetView = nullptr;

	ID3D11DepthStencilView* depthStencilView = nullptr;

	ID3D11RasterizerState* cullFrontState = nullptr;
	ID3D11RasterizerState* cullOnRSState = nullptr;
	ID3D11RasterizerState* wireframeState = nullptr;

	ID3D11BlendState* blendState = nullptr;   

	D3D11_VIEWPORT            viewport;

};