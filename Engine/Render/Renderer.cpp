#include "Renderer.h"
//#include "Level/Level.h"
#include "Common.h"
#include <vector>

Renderer::Renderer(UINT width, UINT height, HWND window)
{
    UINT flag = 0u;

#if _DEBUG
    flag |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D_FEATURE_LEVEL outFeatureLevel;

    // ============================================================================================================================

    ThrowIfFailed(D3D11CreateDevice(nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flag,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &device,
        &outFeatureLevel,
        &context), TEXT("Failed to D3D11CreateDevice"));

    IDXGIFactory* factory = nullptr;
    //CreateDXGIFactory(__uuidof(factory), reinterpret_cast<void**>(&factory));
    ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&factory)),
        TEXT("Failed to create dxgifactory."));

    // ------------------------------
   // Swap Chain.
   // Back Buffer
   // ------------------------------
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = window;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    ThrowIfFailed(factory->CreateSwapChain(device, &sd, &swapChain), TEXT("Failed to CreateSwapChain"));

    // ------------------------------
    // BackBuffer + RenderTargetView
    // ------------------------------
    ID3D11Texture2D* backBuffer = nullptr;
    ThrowIfFailed(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)), TEXT("Failed to GetBuffer"));

    ThrowIfFailed(device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView), TEXT("Failed to CreateRenderTargetView"));

    backBuffer->Release();
    backBuffer = nullptr;

    // ------------------------------
    // DepthStencil Buffer + ViewPort
    // ------------------------------
    ID3D11Texture2D* depthStencilBuffer = nullptr;
    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = width;
    depthStencilDesc.Height = height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ThrowIfFailed(device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer), TEXT("Failed To CreateDepthStencilBuffer"));

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    ThrowIfFailed(
        device->CreateDepthStencilView(
            depthStencilBuffer,
            &depthStencilViewDesc,
            &depthStencilView),
        TEXT("Failed to CreateDepthStencilView"));

    depthStencilBuffer->Release();
    depthStencilBuffer = nullptr;


    // 레스터라이즈
    D3D11_RASTERIZER_DESC rasterizerDesc = { };
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.DepthClipEnable = true;

    ThrowIfFailed(
        device->CreateRasterizerState(&rasterizerDesc, &cullOnRSState),
        TEXT("Failed to create cull on rasterizer state."));

    rasterizerDesc.CullMode = D3D11_CULL_FRONT;

    ThrowIfFailed(
        device->CreateRasterizerState(&rasterizerDesc, &cullFrontState),
        TEXT("Failed to create cull off rasterizer state."));

    // Wireframe 레스터라이저 상태 생성
    rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;

    ThrowIfFailed(
        device->CreateRasterizerState(&rasterizerDesc, &wireframeState),
        TEXT("Failed to create wireframe rasterizer state."));

    context->RSSetState(cullOnRSState);


    // 블렌딩
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    ThrowIfFailed(device->CreateBlendState(&blendDesc, &blendState), TEXT("Failed to CreateBlendState"));

    float blendFactor[4] = { 0.f,0.f,0.f,0.f };
    context->OMSetBlendState(blendState, blendFactor, 0xffffffff);


    // 뷰포트 설정
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = (float)width;
    viewport.Height = (float)height;
    viewport.MaxDepth = 1.0f;
    viewport.MinDepth = 0.0f;

    context->RSSetViewports(1, &viewport);
}

Renderer::~Renderer()
{
    if (context)
    {
        context->Release();
        context = nullptr;
    }
    if (swapChain)
    {
        swapChain->Release();
        swapChain = nullptr;
    }

    if (renderTargetView)
    {
        renderTargetView->Release();
        renderTargetView = nullptr;
    }

    if (depthStencilView)
    {
        depthStencilView->Release();
        depthStencilView = nullptr;
    }

    if (cullFrontState)
    {
        cullFrontState->Release();
        cullFrontState = nullptr;
    }

    if (cullOnRSState)
    {
        cullOnRSState->Release();
        cullOnRSState = nullptr;
    }

    if (wireframeState)
    {
        wireframeState->Release();
        wireframeState = nullptr;
    }

    if (blendState)
    {
        blendState->Release();
        blendState = nullptr;
    }

    if (device)
    {
        device->Release();
        device = nullptr;
    }
}

void Renderer::OnRender()//(std::shared_ptr<Level> level)
{
    if (isResizing)
    {
        return;
    }

    // Shadowmap Pass.
   // DrawToShadowMap(level);

    // 뷰포트 설정.
    context->RSSetViewports(1, &viewport);

    // Phase-1.
    //DrawToRenderTexturePass(level);

    // Final-Phase.
    //DrawFinalPass(level);

      // 화면을 원하는색으로 클리어 
    float color[] = { 0.2f, 0.6f, 0.8f, 1.0f };
    Clear(&renderTargetView, color, depthStencilView);

    // 
    // 화면 전환. (EndScene/Present).
    //swapChain->Present(0u, 0u);

    static ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(3, 1, &nullSRV);
}

void Renderer::OnResize(UINT width, UINT height)
{
    if (!device || !context || !swapChain)
    {
        return;
    }

    isResizing = true;

    context->ClearState();
    context->Flush();

    if (renderTargetView)
    {
        renderTargetView->Release();
        renderTargetView = nullptr;
    }

    if (depthStencilView)
    {
        depthStencilView->Release();
        depthStencilView = nullptr;
    }

    ThrowIfFailed(swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0), TEXT("Failed To ResizeBBuffers"));

    ID3D11Texture2D* backBuffer = nullptr;
    ThrowIfFailed(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)), TEXT("Failed To GetBuffer"));

    ThrowIfFailed(device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView), TEXT("Failed To CreateRenderTargetView"));

    backBuffer->Release();
    backBuffer = nullptr;

    ID3D11Texture2D* depthStencilBuffer = nullptr;
    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = width;
    depthStencilDesc.Height = height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ThrowIfFailed(device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer), TEXT("Failed To CreateDepthStencilBuffer"));

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    ThrowIfFailed(
        device->CreateDepthStencilView(
            depthStencilBuffer,
            &depthStencilViewDesc,
            &depthStencilView),
        TEXT("Failed to CreateDepthStencilView"));

    depthStencilBuffer->Release();
    depthStencilBuffer = nullptr;

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = (float)width;
    viewport.Height = (float)height;
    viewport.MaxDepth = 1.0f;
    viewport.MinDepth = 0.0f;

    context->RSSetViewports(1, &viewport);

    isResizing = false;
}

void Renderer::CullOn()
{
    context->RSSetState(cullOnRSState);
}

void Renderer::CullOff()
{
    context->RSSetState(cullFrontState);
}

void Renderer::WireframeOn()
{
    context->RSSetState(wireframeState);
}

void Renderer::WireframeOff()
{
    context->RSSetState(cullOnRSState);
}

void Renderer::EmptyRTVsAndSRVs()
{
    static ID3D11RenderTargetView* nullRTV = nullptr;
    context->OMSetRenderTargets(1, &nullRTV, nullptr);

    static ID3D11ShaderResourceView* nullSRVs = nullptr;
    context->PSGetShaderResources(0, 1, &nullSRVs);
}

void Renderer::Clear(ID3D11RenderTargetView** renderTargetView, float* clearColor, ID3D11DepthStencilView* depthStencilView)
{
    EmptyRTVsAndSRVs();

    context->OMSetRenderTargets(1, renderTargetView, depthStencilView);

    context->ClearRenderTargetView(*renderTargetView, clearColor);
    context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

//void Renderer::DrawToRenderTexturePass(std::shared_ptr<Level>& level)
//{
//
//}

//void Renderer::DrawFinalPass(std::shared_ptr<Level>& level)
//{
//    // 화면을 원하는색으로 클리어 
//    float color[] = { 0.2f, 0.6f, 0.8f, 1.0f };
//    Clear(&renderTargetView, color, depthStencilView);
//
//    //ID3D11ShaderResourceView* shadowmapSRV = shadowmap->GetShaderResourceView();
//    //context->PSSetShaderResources(3, 1, &shadowmapSRV);
//
//    if (level->GetCamera())
//    {
//        level->GetCamera()->OnRender();
//    }
//
//    if (level->GetLight())
//    {
//        level->GetLight()->OnRender();
//    }
//
//
//    // 이제 모든 Actor 렌더링 (StaticMeshComponent는 비활성화됨)
//    for (UINT ix = 0; ix < level->ActorCount(); ++ix)
//    {
//        auto actor = level->GetActor(ix);
//
//        if (actor && actor->IsActive())
//        {
//            if (actor->IsSkyBox())
//            {
//                CullOff();
//            }
//
//            actor->OnRender();
//
//            CullOn();
//        }
//    }
//}