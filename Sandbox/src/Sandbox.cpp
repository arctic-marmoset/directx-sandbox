#include "gspch.h"
#include "Sandbox/Window.h"

using namespace Microsoft;
using namespace DirectX;

// TYPE DEFINITIONS

struct VERTEX2D
{
    XMFLOAT2 Position;
    XMFLOAT3 Color;
};

union ColorRGBA
{
    struct
    {
        float Red, Green, Blue, Alpha;
    };
    float Raw[4];
};

// CONSTANTS

constexpr UINT kCanvasWidth = 1280;
constexpr UINT kCanvasHeight = 720;

// GLOBALS

WRL::ComPtr<ID3D11Device> g_Device;
WRL::ComPtr<ID3D11DeviceContext> g_Context;
WRL::ComPtr<IDXGISwapChain> g_SwapChain;
WRL::ComPtr<ID3D11RenderTargetView> g_TargetView;

WRL::ComPtr<ID3D11VertexShader> g_VertexShader;
WRL::ComPtr<ID3D11PixelShader> g_PixelShader;

WRL::ComPtr<ID3D11InputLayout> g_InputLayout;
WRL::ComPtr<ID3D11Buffer> g_VertexBuffer;

ColorRGBA g_Background = { 0.15f, 0.2f, 0.4f, 1.0f };
float g_Increment = 0.0001f;

// FUNCTION DECLARATIONS

void D3DInit(HWND windowHandle);
void InitBuffers();
void InitViewport(FLOAT width, FLOAT height);
void InitPipeline();
void InitResources();
void D3DShutdown();

void DrawFrame();

int main(int argc, char *argv[])
{
    Window window(kCanvasWidth, kCanvasHeight);

    D3DInit(window);
    InitBuffers();
    InitViewport(kCanvasWidth, kCanvasHeight);
    InitPipeline();
    InitResources();

    window.ShowWindow(SW_SHOWDEFAULT);

    MSG msg = { };

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            DrawFrame();
        }
    }

    D3DShutdown();

    return static_cast<int>(msg.wParam);
}

// TODO: Move into Window class as Message Handler
LRESULT CALLBACK OnResize(UINT hMsg,
                          WPARAM wParam,
                          LPARAM lParam,
                          BOOL &bHandled)
{
    if (g_SwapChain)
    {
        g_Context->OMSetRenderTargets(0, nullptr, nullptr);
        g_TargetView->Release();
        g_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

        const auto width = LOWORD(lParam);
        const auto height = HIWORD(lParam);

        InitBuffers();
        InitViewport(width, height);
    }
    return 1;
}

void D3DInit(HWND windowHandle)
{
    DXGI_SWAP_CHAIN_DESC scd = { };
    scd.BufferCount = 2;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.Width = kCanvasWidth;
    scd.BufferDesc.Height = kCanvasHeight;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = windowHandle;
    scd.SampleDesc.Count = 1;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    D3D11CreateDeviceAndSwapChain(nullptr,
                                  D3D_DRIVER_TYPE_HARDWARE,
                                  nullptr,
                                  D3D11_CREATE_DEVICE_DEBUG,
                                  nullptr,
                                  0,
                                  D3D11_SDK_VERSION,
                                  &scd,
                                  &g_SwapChain,
                                  &g_Device,
                                  nullptr,
                                  &g_Context);
}

void InitBuffers()
{
    WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
    g_SwapChain->GetBuffer(0,
                           __uuidof(ID3D11Texture2D),
                           &pBackBuffer);

    g_Device->CreateRenderTargetView(pBackBuffer.Get(),
                                     nullptr,
                                     g_TargetView.GetAddressOf());
}

void InitViewport(FLOAT width, FLOAT height)
{
    D3D11_VIEWPORT viewport = { };
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = width;
    viewport.Height = height;

    g_Context->RSSetViewports(1, &viewport);
}

void InitPipeline()
{
    WRL::ComPtr<ID3DBlob> vsBlob;
    WRL::ComPtr<ID3DBlob> psBlob;

    D3DReadFileToBlob(L"Test_VS.cso", &vsBlob);
    D3DReadFileToBlob(L"Test_PS.cso", &psBlob);

    g_Device->CreateVertexShader(vsBlob->GetBufferPointer(),
                                 vsBlob->GetBufferSize(),
                                 nullptr,
                                 &g_VertexShader);

    g_Device->CreatePixelShader(psBlob->GetBufferPointer(),
                                psBlob->GetBufferSize(),
                                nullptr,
                                &g_PixelShader);

    g_Context->VSSetShader(g_VertexShader.Get(), nullptr, 0);
    g_Context->PSSetShader(g_PixelShader.Get(), nullptr, 0);

    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            0,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        },
        {
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D11_APPEND_ALIGNED_ELEMENT,
            D3D11_INPUT_PER_VERTEX_DATA,
            0
        }
    };

    g_Device->CreateInputLayout(ied,
                                2,
                                vsBlob->GetBufferPointer(),
                                vsBlob->GetBufferSize(),
                                &g_InputLayout);

    g_Context->IASetInputLayout(g_InputLayout.Get());
}

void InitResources()
{
    VERTEX2D triangle[] =
    {
        { XMFLOAT2( 0.0f,  0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { XMFLOAT2( 0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { XMFLOAT2(-0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) }
    };

    D3D11_BUFFER_DESC bd = { };
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(triangle);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    g_Device->CreateBuffer(&bd, nullptr, &g_VertexBuffer);

    D3D11_MAPPED_SUBRESOURCE msr = { };
    g_Context->Map(g_VertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
    memcpy(msr.pData, triangle, sizeof(triangle));
    g_Context->Unmap(g_VertexBuffer.Get(), 0);
}

void DrawFrame()
{
    g_Context->OMSetRenderTargets(1, g_TargetView.GetAddressOf(), nullptr);

    g_Context->ClearRenderTargetView(g_TargetView.Get(), g_Background.Raw);
    if (g_Background.Red > 0.9f || g_Background.Red < 0.15f)
    {
        g_Increment = -g_Increment;
    }
    g_Background.Red += g_Increment;

    const UINT stride = sizeof(VERTEX2D);
    const UINT offset = 0;

    g_Context->IASetVertexBuffers(0,
                                  1,
                                  g_VertexBuffer.GetAddressOf(),
                                  &stride,
                                  &offset);

    g_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    g_Context->Draw(3, 0);
    g_SwapChain->Present(0, 0);
}

void D3DShutdown()
{
    g_SwapChain->SetFullscreenState(FALSE, NULL);

    g_Context->Flush();
}
