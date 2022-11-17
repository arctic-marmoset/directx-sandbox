#pragma once

class Window;

namespace DX11
{
    class Graphics final
    {
    public:
        Graphics();
        ~Graphics();

        void SetWindow(Window &window);
        void SetDimensions(std::uint16_t width, std::uint16_t height);

        void Clear(float red, float green, float blue);
        void BeginFrame();
        void EndFrame();

        // Temporary "debug" method
        void DrawCube(float step);

        ID3D11Device2 *GetD3DDevice() const
        {
            return m_Device.Get();
        }

        ID3D11DeviceContext2 *GetD3DContext() const
        {
            return m_Context.Get();
        }

        Graphics(const Graphics &) = delete;
        Graphics &operator=(const Graphics &) = delete;

    private:
        void InitDeviceResources();
        void InitDimensionDependentResources();

    private:
        Window *m_Window{ nullptr };

        Microsoft::WRL::ComPtr<ID3D11Device2>        m_Device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext2> m_Context;
        Microsoft::WRL::ComPtr<IDXGISwapChain1>      m_SwapChain;

        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
        D3D11_VIEWPORT                                 m_RenderViewport{ };

        D3D_FEATURE_LEVEL m_FeatureLevel{ };

        std::uint16_t m_LogicalWidth { 1U };
        std::uint16_t m_LogicalHeight{ 1U };
        std::uint8_t  m_SyncInterval { 0U };
    };
}
