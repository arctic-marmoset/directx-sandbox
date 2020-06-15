#pragma once

#include <chrono>

namespace DX11
{

    class Graphics final
    {
    public:
        Graphics(HWND windowHandle);
        Graphics(const Graphics &) = delete;
        ~Graphics();

        void OnResize(float width, float height);

        void SetViewport(float width, float height);
        
        void InitBackBuffer();
        void ClearBackBuffer(float red = 0.0f, float green = 0.0f, float blue = 0.0f);

        void BeginFrame();
        void EndFrame();

        // Temporary "debug" method
        void DrawCube();

        Graphics &operator=(const Graphics &) = delete;

    private:
        void InitDeviceAndSwapChain(HWND windowHandle);

        std::chrono::steady_clock::time_point m_TimeLast;

        Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;
        Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_Target;
    };

}
