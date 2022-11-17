#include "gsspch.hpp"
#include "Graphics.hpp"

#include "Sandbox/Window.hpp"

#include <array>

namespace Wrl = Microsoft::WRL;
namespace Dx  = DirectX;

DX11::Graphics::Graphics()
{
    InitDeviceResources();
}

DX11::Graphics::~Graphics()
{
    if (m_SwapChain)
    {
        m_SwapChain->SetFullscreenState(FALSE, nullptr);
    }
}

void DX11::Graphics::InitDeviceResources()
{
    constexpr std::array featureLevels = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    UINT createDeviceFlags = 0;

#ifndef NDEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    Wrl::ComPtr<ID3D11Device> device;
    Wrl::ComPtr<ID3D11DeviceContext> context;

    constexpr UINT featureLevelCount{ featureLevels.size() };

    const HRESULT createDeviceResult = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        featureLevels.data(),
        featureLevelCount,
        D3D11_SDK_VERSION,
        device.ReleaseAndGetAddressOf(),
        &m_FeatureLevel,
        context.ReleaseAndGetAddressOf()
    );

    if (FAILED(createDeviceResult))
    {
        // TODO: Handle error
    }

    // Store device and context pointers.
    device.As(&m_Device);
    context.As(&m_Context);
}

void DX11::Graphics::SetWindow(Window &window)
{
    m_Window = &window;
    SetDimensions(m_Window->GetWidth(), m_Window->GetHeight());
}

void DX11::Graphics::SetDimensions(std::uint16_t width, std::uint16_t height)
{
    if (m_LogicalWidth != width || m_LogicalHeight != height)
    {
        m_LogicalWidth  = width;
        m_LogicalHeight = height;
        InitDimensionDependentResources();
    }
}

void DX11::Graphics::InitDimensionDependentResources()
{
    constexpr std::array<ID3D11RenderTargetView *, 1> nullViews = { nullptr };

    constexpr UINT nullViewCount{ nullViews.size() };
    m_Context->OMSetRenderTargets(nullViewCount, nullViews.data(), nullptr);
    m_RenderTargetView = nullptr;
    m_DepthStencilView = nullptr;
    m_Context->Flush();

    if (m_SwapChain)
    {
        const HRESULT resizeBuffersResult = m_SwapChain->ResizeBuffers(
            0,
            m_LogicalWidth,
            m_LogicalHeight,
            DXGI_FORMAT_UNKNOWN,
            0
        );

        if (FAILED(resizeBuffersResult))
        {
            throw std::runtime_error("failed to resize buffers");
        }
    }
    else
    {
        // Swapchain does not exist, so create a new one.
        const DXGI_SWAP_CHAIN_DESC1 scd{
            .Width       = m_LogicalWidth,
            .Height      = m_LogicalHeight,
            .Format      = DXGI_FORMAT_B8G8R8A8_UNORM,
            .Stereo      = FALSE,
            .SampleDesc  = {
                .Count   = 1,
                .Quality = 0,
            },
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = 2,
            .Scaling     = DXGI_SCALING_NONE,
            .SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .AlphaMode   = DXGI_ALPHA_MODE_UNSPECIFIED,
            .Flags       = 0
        };

        const DXGI_SWAP_CHAIN_FULLSCREEN_DESC scfsd{
            .Windowed = TRUE,
        };

        // Get the DXGIFactory that created m_Device
        Wrl::ComPtr<IDXGIDevice3> dxgiDevice;
        m_Device.As(&dxgiDevice);

        Wrl::ComPtr<IDXGIAdapter> dxgiAdapter;
        dxgiDevice->GetAdapter(&dxgiAdapter);

        Wrl::ComPtr<IDXGIFactory3> dxgiFactory;
        dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

        const HRESULT createSwapChainResult = dxgiFactory->CreateSwapChainForHwnd(
            m_Device.Get(),
            m_Window->GetHandle(),
            &scd,
            &scfsd,
            nullptr,
            m_SwapChain.ReleaseAndGetAddressOf()
        );

        if (FAILED(createSwapChainResult))
        {
            throw std::runtime_error("failed to create swapchain");
        }
    }

    // Create a render target view of the swapchain backbuffer.
    Wrl::ComPtr<ID3D11Texture2D> backBuffer;
    m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

    m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_RenderTargetView.ReleaseAndGetAddressOf());

    // Create a depth stencil buffer view.
    const D3D11_TEXTURE2D_DESC dsd{
        .Width      = m_LogicalWidth,
        .Height     = m_LogicalHeight,
        .MipLevels  = 1,
        .ArraySize  = 1,
        .Format     = DXGI_FORMAT_D32_FLOAT,
        .SampleDesc = {
            .Count   = 1,
            .Quality = 0,
        },
        .Usage      = D3D11_USAGE_DEFAULT,
        .BindFlags  = D3D11_BIND_DEPTH_STENCIL,
    };

    Wrl::ComPtr<ID3D11Texture2D> depthStencil;
    m_Device->CreateTexture2D(&dsd, nullptr, &depthStencil);

    const CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(D3D11_DSV_DIMENSION_TEXTURE2D);
    m_Device->CreateDepthStencilView(
        depthStencil.Get(),
        &dsvd,
        &m_DepthStencilView
    );

    // Set render viewport.
    m_RenderViewport = CD3D11_VIEWPORT(
        0.0f,
        0.0f,
        m_LogicalWidth,
        m_LogicalHeight
    );

    m_Context->RSSetViewports(1, &m_RenderViewport);
}

void DX11::Graphics::Clear(float red, float green, float blue)
{
    const float color[] = { red, green, blue, 1.0f };
    m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), color);
    m_Context->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DX11::Graphics::BeginFrame()
{
    m_Context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());
}

void DX11::Graphics::EndFrame()
{
    const HRESULT presentResult = m_SwapChain->Present(m_SyncInterval, 0);

    switch (presentResult)
    {
    case DXGI_STATUS_OCCLUDED:
        [[fallthrough]];
    case S_OK:
        break;
    default:
        throw std::runtime_error("failed to present");
    }
}

struct VERTEX3D
{
    Dx::XMFLOAT3 Pos;
    Dx::XMFLOAT3 Color;
};

static constexpr VERTEX3D cube[] = {
    { Dx::XMFLOAT3(-1.0f,  1.0f, -1.0f), Dx::XMFLOAT3( 0.0f,  1.0f,  0.0f) }, //0
    { Dx::XMFLOAT3(-1.0f,  1.0f,  1.0f), Dx::XMFLOAT3( 0.0f,  1.0f,  0.0f) }, //4
    { Dx::XMFLOAT3( 1.0f,  1.0f, -1.0f), Dx::XMFLOAT3( 0.0f,  1.0f,  0.0f) }, //1
                                                                     
    { Dx::XMFLOAT3( 1.0f,  1.0f,  1.0f), Dx::XMFLOAT3( 0.0f,  1.0f,  0.0f) }, //5
    { Dx::XMFLOAT3( 1.0f,  1.0f, -1.0f), Dx::XMFLOAT3( 0.0f,  1.0f,  0.0f) }, //1
    { Dx::XMFLOAT3(-1.0f,  1.0f,  1.0f), Dx::XMFLOAT3( 0.0f,  1.0f,  0.0f) }, //4
                                                              
    { Dx::XMFLOAT3(-1.0f,  1.0f, -1.0f), Dx::XMFLOAT3( 0.0f,  0.0f, -1.0f) }, //0
    { Dx::XMFLOAT3( 1.0f,  1.0f, -1.0f), Dx::XMFLOAT3( 0.0f,  0.0f, -1.0f) }, //1
    { Dx::XMFLOAT3(-1.0f, -1.0f, -1.0f), Dx::XMFLOAT3( 0.0f,  0.0f, -1.0f) }, //2
                                                              
    { Dx::XMFLOAT3( 1.0f, -1.0f, -1.0f), Dx::XMFLOAT3( 0.0f,  0.0f, -1.0f) }, //3
    { Dx::XMFLOAT3(-1.0f, -1.0f, -1.0f), Dx::XMFLOAT3( 0.0f,  0.0f, -1.0f) }, //2
    { Dx::XMFLOAT3( 1.0f,  1.0f, -1.0f), Dx::XMFLOAT3( 0.0f,  0.0f, -1.0f) }, //1
                                                              
    { Dx::XMFLOAT3(-1.0f,  1.0f, -1.0f), Dx::XMFLOAT3(-1.0f,  0.0f,  0.0f) }, //0
    { Dx::XMFLOAT3(-1.0f, -1.0f, -1.0f), Dx::XMFLOAT3(-1.0f,  0.0f,  0.0f) }, //2
    { Dx::XMFLOAT3(-1.0f,  1.0f,  1.0f), Dx::XMFLOAT3(-1.0f,  0.0f,  0.0f) }, //4
                                                                     
    { Dx::XMFLOAT3(-1.0f, -1.0f,  1.0f), Dx::XMFLOAT3(-1.0f,  0.0f,  0.0f) }, //6
    { Dx::XMFLOAT3(-1.0f,  1.0f,  1.0f), Dx::XMFLOAT3(-1.0f,  0.0f,  0.0f) }, //4
    { Dx::XMFLOAT3(-1.0f, -1.0f, -1.0f), Dx::XMFLOAT3(-1.0f,  0.0f,  0.0f) }, //2

    { Dx::XMFLOAT3( 1.0f,  1.0f, -1.0f), Dx::XMFLOAT3( 1.0f,  0.0f,  0.0f) }, //1
    { Dx::XMFLOAT3( 1.0f,  1.0f,  1.0f), Dx::XMFLOAT3( 1.0f,  0.0f,  0.0f) }, //5
    { Dx::XMFLOAT3( 1.0f, -1.0f, -1.0f), Dx::XMFLOAT3( 1.0f,  0.0f,  0.0f) }, //3

    { Dx::XMFLOAT3( 1.0f, -1.0f,  1.0f), Dx::XMFLOAT3( 1.0f,  0.0f,  0.0f) }, //7
    { Dx::XMFLOAT3( 1.0f, -1.0f, -1.0f), Dx::XMFLOAT3( 1.0f,  0.0f,  0.0f) }, //3
    { Dx::XMFLOAT3( 1.0f,  1.0f,  1.0f), Dx::XMFLOAT3( 1.0f,  0.0f,  0.0f) }, //5

    { Dx::XMFLOAT3( 1.0f, -1.0f, -1.0f), Dx::XMFLOAT3( 0.0f, -1.0f,  0.0f) }, //3
    { Dx::XMFLOAT3( 1.0f, -1.0f,  1.0f), Dx::XMFLOAT3( 0.0f, -1.0f,  0.0f) }, //7
    { Dx::XMFLOAT3(-1.0f, -1.0f, -1.0f), Dx::XMFLOAT3( 0.0f, -1.0f,  0.0f) }, //2

    { Dx::XMFLOAT3(-1.0f, -1.0f,  1.0f), Dx::XMFLOAT3( 0.0f, -1.0f,  0.0f) }, //6
    { Dx::XMFLOAT3(-1.0f, -1.0f, -1.0f), Dx::XMFLOAT3( 0.0f, -1.0f,  0.0f) }, //2
    { Dx::XMFLOAT3( 1.0f, -1.0f,  1.0f), Dx::XMFLOAT3( 0.0f, -1.0f,  0.0f) }, //7

    { Dx::XMFLOAT3(-1.0f,  1.0f,  1.0f), Dx::XMFLOAT3( 0.0f,  0.0f,  1.0f) }, //4
    { Dx::XMFLOAT3(-1.0f, -1.0f,  1.0f), Dx::XMFLOAT3( 0.0f,  0.0f,  1.0f) }, //6
    { Dx::XMFLOAT3( 1.0f,  1.0f,  1.0f), Dx::XMFLOAT3( 0.0f,  0.0f,  1.0f) }, //5

    { Dx::XMFLOAT3( 1.0f, -1.0f,  1.0f), Dx::XMFLOAT3( 0.0f,  0.0f,  1.0f) }, //7
    { Dx::XMFLOAT3( 1.0f,  1.0f,  1.0f), Dx::XMFLOAT3( 0.0f,  0.0f,  1.0f) }, //5
    { Dx::XMFLOAT3(-1.0f, -1.0f,  1.0f), Dx::XMFLOAT3( 0.0f,  0.0f,  1.0f) }, //6
};

void DX11::Graphics::DrawCube(float step)
{
    // Initialise Vertex Buffer
    const D3D11_BUFFER_DESC vbd{
        .ByteWidth           = sizeof(cube),
        .Usage               = D3D11_USAGE_DEFAULT,
        .BindFlags           = D3D11_BIND_VERTEX_BUFFER,
        .StructureByteStride = sizeof(cube[0]),
    };

    const D3D11_SUBRESOURCE_DATA vsrd{
        .pSysMem = cube,
    };

    Wrl::ComPtr<ID3D11Buffer> vertexBuffer;
    m_Device->CreateBuffer(&vbd, &vsrd, &vertexBuffer);

    // Bind Vertex Buffer to IA stage

    const UINT stride = sizeof(VERTEX3D);
    const UINT offset = 0;

    m_Context->IASetVertexBuffers(
        0,
        1,
        vertexBuffer.GetAddressOf(),
        &stride,
        &offset
    );

    struct ConstBuffer
    {
        Dx::XMMATRIX World;
        Dx::XMMATRIX WorldViewProj;
    };

    UINT vpCount = 1;
    D3D11_VIEWPORT vp;
    m_Context->RSGetViewports(&vpCount, &vp);

    Dx::XMMATRIX model = Dx::XMMatrixRotationX(step / 4.0f) *
                         Dx::XMMatrixRotationY(step) *
                         Dx::XMMatrixTranslation(0.0f, 0.0f, 5.0f);

    const ConstBuffer cb{
        .World         = Dx::XMMatrixTranspose(model),
        .WorldViewProj = Dx::XMMatrixTranspose(
            model * Dx::XMMatrixPerspectiveFovLH(
                Dx::XMConvertToRadians(47), 
                vp.Width / vp.Height, 
                0.05f, 
                100.0f
            )
        ),
    };

    const D3D11_BUFFER_DESC cbd{
        .ByteWidth           = sizeof(cb),
        .Usage               = D3D11_USAGE_DEFAULT,
        .BindFlags           = D3D11_BIND_CONSTANT_BUFFER,
        .StructureByteStride = 0,
    };

    const D3D11_SUBRESOURCE_DATA csrd{
        .pSysMem = &cb,
    };

    Wrl::ComPtr<ID3D11Buffer> constantBuffer;
    m_Device->CreateBuffer(&cbd, &csrd, &constantBuffer);

    m_Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

    m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    static_assert(!(std::size(cube) > std::numeric_limits<UINT>::max()));
    m_Context->Draw(static_cast<UINT>(std::size(cube)), 0);
}
