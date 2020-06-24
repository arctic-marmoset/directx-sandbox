#pragma once

namespace DX11
{

    class Graphics final
    {
    public:
        Graphics();
        Graphics(const Graphics &) = delete;
        ~Graphics();

        void SetWindow(HWND windowHandle);
        void SetDimensions(int width, int height);

        void Clear(float red = 0.0f, float green = 0.0f, float blue = 0.0f);
        void BeginFrame();
        void EndFrame();

        // Temporary "debug" method
        void DrawCube(float step);

        ID3D11Device2 *GetD3DDevice() const         { return m_Device.Get(); }
        ID3D11DeviceContext2 *GetD3DContext() const { return m_Context.Get(); }

        Graphics &operator=(const Graphics &) = delete;

    private:
        void InitDeviceResources();
        void InitDimensionDependentResources();

    private:
        HWND m_WindowHandle;

        Microsoft::WRL::ComPtr<ID3D11Device2>        m_Device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext2> m_Context;
        Microsoft::WRL::ComPtr<IDXGISwapChain1>      m_SwapChain;

        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
        D3D11_VIEWPORT                                 m_RenderViewport;

        D3D_FEATURE_LEVEL m_FeatureLevel;

        int m_LogicalWidth;
        int m_LogicalHeight;
    };

}
