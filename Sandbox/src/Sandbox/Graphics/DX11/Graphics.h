#pragma once

namespace DX11
{

    class Graphics final
    {
    public:
        Graphics(HWND windowHandle);
        Graphics(const Graphics &) = delete;
        ~Graphics();

        void OnResize(float width, float height);
        void SetViewport(float width = 0.0f, float height = 0.0f);

        void Clear(float red = 0.0f, float green = 0.0f, float blue = 0.0f);
        void BeginFrame();
        void EndFrame();

        // Temporary "debug" method
        void DrawCube(float step);

        Graphics &operator=(const Graphics &) = delete;

    private:
        void InitDeviceAndSwapChain();
        void InitBackBuffer();
        void InitDepthStencilBuffer();

    private:
        HWND m_WindowHandle;

        Microsoft::WRL::ComPtr<ID3D11Device>        m_Device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext;
        Microsoft::WRL::ComPtr<IDXGISwapChain>      m_SwapChain;

        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
    };

}
