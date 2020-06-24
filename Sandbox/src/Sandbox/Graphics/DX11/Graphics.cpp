#include "gspch.h"
#include "Graphics.h"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

namespace DX11
{

    Graphics::Graphics()
        :
        m_WindowHandle(nullptr),
        m_LogicalWidth(0),
        m_LogicalHeight(0),
        m_SyncInterval(0)
    {
        InitDeviceResources();
    }

    Graphics::~Graphics()
    {
        m_SwapChain->SetFullscreenState(FALSE, nullptr);
    }

    void Graphics::InitDeviceResources()
    {
        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        UINT createDeviceFlags = 0;

    #ifndef NDEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

        wrl::ComPtr<ID3D11Device> device;
        wrl::ComPtr<ID3D11DeviceContext> context;

        HRESULT hr = D3D11CreateDevice(nullptr,
                                       D3D_DRIVER_TYPE_HARDWARE,
                                       0,
                                       createDeviceFlags,
                                       featureLevels,
                                       ARRAYSIZE(featureLevels),
                                       D3D11_SDK_VERSION,
                                       &device,
                                       &m_FeatureLevel,
                                       &context);

        if (FAILED(hr))
        {
            // TODO: Handle error
        }

        // Store device and context pointers.
        device.As(&m_Device);
        context.As(&m_Context);
    }

    void Graphics::SetWindow(HWND windowHandle)
    {
        m_WindowHandle = windowHandle;

        RECT cr = { };
        GetClientRect(windowHandle, &cr);
        m_LogicalWidth = cr.right - cr.left;
        m_LogicalHeight = cr.bottom - cr.top;
        InitDimensionDependentResources();
    }

    void Graphics::SetDimensions(int width, int height)
    {
        if (m_LogicalWidth != width || m_LogicalHeight != height)
        {
            m_LogicalWidth = width;
            m_LogicalHeight = height;
            InitDimensionDependentResources();
        }
    }

    void Graphics::InitDimensionDependentResources()
    {
        ID3D11RenderTargetView *nullViews[] = { nullptr };
        m_Context->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
        m_RenderTargetView = nullptr;
        m_DepthStencilView = nullptr;
        m_Context->Flush();

        if (m_SwapChain)
        {
            HRESULT hr = m_SwapChain->ResizeBuffers(0,
                                                    m_LogicalWidth,
                                                    m_LogicalHeight,
                                                    DXGI_FORMAT_UNKNOWN,
                                                    0);

            if (FAILED(hr))
            {
                // TODO: Handle error
            }
        }
        else
        {
            // Swapchain does not exist, so create a new one.
            DXGI_SWAP_CHAIN_DESC1 scd = { };
            scd.Width = m_LogicalWidth;
            scd.Height = m_LogicalHeight;
            scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            scd.SampleDesc.Count = 1;
            scd.SampleDesc.Quality = 0;
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            scd.BufferCount = 2;
            scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

            DXGI_SWAP_CHAIN_FULLSCREEN_DESC scfsd = { };
            scfsd.Windowed = TRUE;

            // Get the DXGIFactory that created m_Device
            wrl::ComPtr<IDXGIDevice3> dxgiDevice;
            m_Device.As(&dxgiDevice);

            wrl::ComPtr<IDXGIAdapter> dxgiAdapter;
            dxgiDevice->GetAdapter(&dxgiAdapter);

            wrl::ComPtr<IDXGIFactory3> dxgiFactory;
            dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

            dxgiFactory->CreateSwapChainForHwnd(m_Device.Get(),
                                                m_WindowHandle,
                                                &scd,
                                                &scfsd,
                                                nullptr,
                                                &m_SwapChain);

            dxgiDevice->SetMaximumFrameLatency(1);
        }

        // Create a render target view of the swapchain backbuffer.
        wrl::ComPtr<ID3D11Texture2D> backBuffer;
        m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

        m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_RenderTargetView);

        // Create a depth stencil buffer view.
        D3D11_TEXTURE2D_DESC dsd = { };
        dsd.Width = m_LogicalWidth;
        dsd.Height = m_LogicalHeight;
        dsd.MipLevels = 1;
        dsd.ArraySize = 1;
        dsd.Format = DXGI_FORMAT_D32_FLOAT;
        dsd.SampleDesc.Count = 1;
        dsd.SampleDesc.Quality = 0;
        dsd.Usage = D3D11_USAGE_DEFAULT;
        dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        wrl::ComPtr<ID3D11Texture2D> depthStencil;
        m_Device->CreateTexture2D(&dsd, nullptr, &depthStencil);

        CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(D3D11_DSV_DIMENSION_TEXTURE2D);
        m_Device->CreateDepthStencilView(depthStencil.Get(),
                                         &dsvd,
                                         &m_DepthStencilView);

        // Set render viewport.
        m_RenderViewport = CD3D11_VIEWPORT(0.0f, 
                                           0.0f, 
                                           static_cast<FLOAT>(m_LogicalWidth), 
                                           static_cast<FLOAT>(m_LogicalHeight));

        m_Context->RSSetViewports(1, &m_RenderViewport);
    }

    void Graphics::Clear(float red, float green, float blue)
    {
        const float color[] = { red, green, blue, 1.0f };
        m_Context->ClearRenderTargetView(m_RenderTargetView.Get(), color);
        m_Context->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    }

    void Graphics::BeginFrame()
    {
        m_Context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());
    }

    void Graphics::EndFrame()
    {
        HRESULT hr = m_SwapChain->Present(m_SyncInterval, 0);
        if (FAILED(hr))
        {
            // TODO: Handle error
        }
    }

    void Graphics::DrawCube(float step)
    {
        // Define vertex resources

        struct VERTEX3D
        {
            dx::XMFLOAT3 Pos;
            dx::XMFLOAT3 Color;
        };

        // TODO: Stop hard-coding geometries
        VERTEX3D cube[] =
        {
            { dx::XMFLOAT3(-1.0f,  1.0f, -1.0f), dx::XMFLOAT3( 0.0f,  1.0f,  0.0f) }, //0
            { dx::XMFLOAT3(-1.0f,  1.0f,  1.0f), dx::XMFLOAT3( 0.0f,  1.0f,  0.0f) }, //4
            { dx::XMFLOAT3( 1.0f,  1.0f, -1.0f), dx::XMFLOAT3( 0.0f,  1.0f,  0.0f) }, //1
                                                                             
            { dx::XMFLOAT3( 1.0f,  1.0f,  1.0f), dx::XMFLOAT3( 0.0f,  1.0f,  0.0f) }, //5
            { dx::XMFLOAT3( 1.0f,  1.0f, -1.0f), dx::XMFLOAT3( 0.0f,  1.0f,  0.0f) }, //1
            { dx::XMFLOAT3(-1.0f,  1.0f,  1.0f), dx::XMFLOAT3( 0.0f,  1.0f,  0.0f) }, //4
                                                                      
            { dx::XMFLOAT3(-1.0f,  1.0f, -1.0f), dx::XMFLOAT3( 0.0f,  0.0f, -1.0f) }, //0
            { dx::XMFLOAT3( 1.0f,  1.0f, -1.0f), dx::XMFLOAT3( 0.0f,  0.0f, -1.0f) }, //1
            { dx::XMFLOAT3(-1.0f, -1.0f, -1.0f), dx::XMFLOAT3( 0.0f,  0.0f, -1.0f) }, //2
                                                                      
            { dx::XMFLOAT3( 1.0f, -1.0f, -1.0f), dx::XMFLOAT3( 0.0f,  0.0f, -1.0f) }, //3
            { dx::XMFLOAT3(-1.0f, -1.0f, -1.0f), dx::XMFLOAT3( 0.0f,  0.0f, -1.0f) }, //2
            { dx::XMFLOAT3( 1.0f,  1.0f, -1.0f), dx::XMFLOAT3( 0.0f,  0.0f, -1.0f) }, //1
                                                                      
            { dx::XMFLOAT3(-1.0f,  1.0f, -1.0f), dx::XMFLOAT3(-1.0f,  0.0f,  0.0f) }, //0
            { dx::XMFLOAT3(-1.0f, -1.0f, -1.0f), dx::XMFLOAT3(-1.0f,  0.0f,  0.0f) }, //2
            { dx::XMFLOAT3(-1.0f,  1.0f,  1.0f), dx::XMFLOAT3(-1.0f,  0.0f,  0.0f) }, //4
                                                                             
            { dx::XMFLOAT3(-1.0f, -1.0f,  1.0f), dx::XMFLOAT3(-1.0f,  0.0f,  0.0f) }, //6
            { dx::XMFLOAT3(-1.0f,  1.0f,  1.0f), dx::XMFLOAT3(-1.0f,  0.0f,  0.0f) }, //4
            { dx::XMFLOAT3(-1.0f, -1.0f, -1.0f), dx::XMFLOAT3(-1.0f,  0.0f,  0.0f) }, //2

            { dx::XMFLOAT3( 1.0f,  1.0f, -1.0f), dx::XMFLOAT3( 1.0f,  0.0f,  0.0f) }, //1
            { dx::XMFLOAT3( 1.0f,  1.0f,  1.0f), dx::XMFLOAT3( 1.0f,  0.0f,  0.0f) }, //5
            { dx::XMFLOAT3( 1.0f, -1.0f, -1.0f), dx::XMFLOAT3( 1.0f,  0.0f,  0.0f) }, //3

            { dx::XMFLOAT3( 1.0f, -1.0f,  1.0f), dx::XMFLOAT3( 1.0f,  0.0f,  0.0f) }, //7
            { dx::XMFLOAT3( 1.0f, -1.0f, -1.0f), dx::XMFLOAT3( 1.0f,  0.0f,  0.0f) }, //3
            { dx::XMFLOAT3( 1.0f,  1.0f,  1.0f), dx::XMFLOAT3( 1.0f,  0.0f,  0.0f) }, //5

            { dx::XMFLOAT3( 1.0f, -1.0f, -1.0f), dx::XMFLOAT3( 0.0f, -1.0f,  0.0f) }, //3
            { dx::XMFLOAT3( 1.0f, -1.0f,  1.0f), dx::XMFLOAT3( 0.0f, -1.0f,  0.0f) }, //7
            { dx::XMFLOAT3(-1.0f, -1.0f, -1.0f), dx::XMFLOAT3( 0.0f, -1.0f,  0.0f) }, //2

            { dx::XMFLOAT3(-1.0f, -1.0f,  1.0f), dx::XMFLOAT3( 0.0f, -1.0f,  0.0f) }, //6
            { dx::XMFLOAT3(-1.0f, -1.0f, -1.0f), dx::XMFLOAT3( 0.0f, -1.0f,  0.0f) }, //2
            { dx::XMFLOAT3( 1.0f, -1.0f,  1.0f), dx::XMFLOAT3( 0.0f, -1.0f,  0.0f) }, //7

            { dx::XMFLOAT3(-1.0f,  1.0f,  1.0f), dx::XMFLOAT3( 0.0f,  0.0f,  1.0f) }, //4
            { dx::XMFLOAT3(-1.0f, -1.0f,  1.0f), dx::XMFLOAT3( 0.0f,  0.0f,  1.0f) }, //6
            { dx::XMFLOAT3( 1.0f,  1.0f,  1.0f), dx::XMFLOAT3( 0.0f,  0.0f,  1.0f) }, //5

            { dx::XMFLOAT3( 1.0f, -1.0f,  1.0f), dx::XMFLOAT3( 0.0f,  0.0f,  1.0f) }, //7
            { dx::XMFLOAT3( 1.0f,  1.0f,  1.0f), dx::XMFLOAT3( 0.0f,  0.0f,  1.0f) }, //5
            { dx::XMFLOAT3(-1.0f, -1.0f,  1.0f), dx::XMFLOAT3( 0.0f,  0.0f,  1.0f) }, //6
        };

        // Initialise Vertex Buffer

        D3D11_BUFFER_DESC vbd = { };
        vbd.Usage = D3D11_USAGE_DEFAULT;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.ByteWidth = sizeof(cube);
        vbd.StructureByteStride = sizeof(VERTEX3D);

        D3D11_SUBRESOURCE_DATA vsrd = { };
        vsrd.pSysMem = cube;

        wrl::ComPtr<ID3D11Buffer> vertexBuffer;
        m_Device->CreateBuffer(&vbd, &vsrd, &vertexBuffer);

        // Bind Vertex Buffer to IA stage

        const UINT stride = sizeof(VERTEX3D);
        const UINT offset = 0;

        m_Context->IASetVertexBuffers(0,
                                      1,
                                      vertexBuffer.GetAddressOf(),
                                      &stride,
                                      &offset);

        // Cannot use indices (for now) if normals are not interpolated

        // Define indices
        /*
        const unsigned short indices[] = 
        { 
            0, 4, 1,   5, 1, 4,
            0, 1, 2,   3, 2, 1,
            0, 2, 4,   6, 4, 2,
            1, 5, 3,   7, 3, 5,
            3, 7, 2,   6, 2, 7,
            4, 6, 5,   7, 5, 6,
        };

        // Initialise Index Buffer

        D3D11_BUFFER_DESC ibd = { };
        ibd.Usage = D3D11_USAGE_DEFAULT;
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.ByteWidth = sizeof(indices);
        ibd.StructureByteStride = sizeof(unsigned short);

        D3D11_SUBRESOURCE_DATA isrd = { };
        isrd.pSysMem = indices;

        wrl::ComPtr<ID3D11Buffer> indexBuffer;
        m_Device->CreateBuffer(&ibd, &isrd, &indexBuffer);

        // Bind Index Buffer to IA stage

        m_Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
        */

        // Define constant buffer

        struct ConstBuffer
        {
            dx::XMMATRIX World;
            dx::XMMATRIX WorldViewProj;
        };

        UINT vpCount = 1;
        D3D11_VIEWPORT vp = { };
        m_Context->RSGetViewports(&vpCount, &vp);

        dx::XMMATRIX model = dx::XMMatrixRotationX(step / 4.0f) *
                             dx::XMMatrixRotationY(step) *
                             dx::XMMatrixTranslation(0.0f, 0.0f, 5.0f);

        const ConstBuffer cb =
        {
            dx::XMMatrixTranspose(model),

            dx::XMMatrixTranspose(model *
                                  dx::XMMatrixPerspectiveFovLH(dx::XMConvertToRadians(47), vp.Width / vp.Height, 0.05f, 100.0f))
        };

        // Initialise Constant Buffer

        D3D11_BUFFER_DESC cbd = { };
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.ByteWidth = sizeof(cb);
        cbd.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA csrd = { };
        csrd.pSysMem = &cb;

        wrl::ComPtr<ID3D11Buffer> constantBuffer;
        m_Device->CreateBuffer(&cbd, &csrd, &constantBuffer);

        // Bind Constant Buffer to VS stage

        m_Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

        // Describe topology

        m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Finally

        //m_Context->DrawIndexed(static_cast<UINT>(std::size(indices)), 0, 0);
        m_Context->Draw(static_cast<UINT>(std::size(cube)), 0);
    }

}
