#include "gspch.h"

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
constexpr LPCWSTR kWndClassName = L"WindowClass";

// GLOBALS

WRL::ComPtr<ID3D11Device> g_Device;
WRL::ComPtr<ID3D11DeviceContext> g_Context;
WRL::ComPtr<IDXGISwapChain> g_SwapChain;
WRL::ComPtr<ID3D11RenderTargetView> g_BackBuffer;

WRL::ComPtr<ID3D11VertexShader> g_VertexShader;
WRL::ComPtr<ID3D11PixelShader> g_PixelShader;

ColorRGBA g_Background = { 0.15f, 0.2f, 0.4f, 1.0f };
float g_Increment = 0.0001f;

// FUNCTION DECLARATIONS

void D3DInit(HWND windowHandle);
void InitPipeline();
void D3DShutdown();

void DrawFrame();

LRESULT CALLBACK WindowProc(HWND windowHandle,
                            UINT message,
                            WPARAM wParam,
                            LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow);

int main(int argc, char *argv[])
{
    return WinMain(GetModuleHandle(nullptr),
                   nullptr,
                   GetCommandLineA(),
                   SW_SHOWDEFAULT);
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    HWND hWnd = nullptr;

    WNDCLASSEX wc = { };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = kWndClassName;

    RegisterClassEx(&wc);

    RECT wr = { 0, 0, kCanvasWidth, kCanvasHeight };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindowEx(0,
                          kWndClassName,
                          L"DirectX Program",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          wr.right - wr.left,
                          wr.bottom - wr.top,
                          nullptr,
                          nullptr,
                          hInstance,
                          nullptr);

    D3DInit(hWnd);
    InitPipeline();
    ShowWindow(hWnd, nCmdShow);

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
    UnregisterClass(kWndClassName, hInstance);

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hWnd,
                            UINT message,
                            WPARAM wParam,
                            LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
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

    WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
    g_SwapChain->GetBuffer(0,
                           __uuidof(ID3D11Texture2D),
                           &pBackBuffer);

    g_Device->CreateRenderTargetView(pBackBuffer.Get(),
                                     nullptr,
                                     &g_BackBuffer);

    g_Context->OMSetRenderTargets(1, g_BackBuffer.GetAddressOf(), nullptr);

    D3D11_VIEWPORT viewport = { };
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = kCanvasWidth;
    viewport.Height = kCanvasHeight;

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
}

void DrawFrame()
{
    g_Context->ClearRenderTargetView(g_BackBuffer.Get(), g_Background.Raw);
    if (g_Background.Red > 0.9f || g_Background.Red < 0.15f)
    {
        g_Increment = -g_Increment;
    }
    g_Background.Red += g_Increment;

    g_SwapChain->Present(0, 0);
}

void D3DShutdown()
{
    g_SwapChain->SetFullscreenState(FALSE, NULL);

    g_Context->Flush();
}
