#include "gspch.h"
#include "Graphics.h"

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

namespace DX11
{

    Graphics::Graphics(HWND windowHandle)
    {
        InitDeviceAndSwapChain(windowHandle);
        InitBackBuffer();
    }

    void Graphics::InitDeviceAndSwapChain(HWND windowHandle)
    {
        DXGI_SWAP_CHAIN_DESC scd = { };
        scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        scd.SampleDesc.Count = 1;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.BufferCount = 2;
        scd.OutputWindow = windowHandle;
        scd.Windowed = TRUE;
        scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        UINT createDeviceFlags = 0;

    #ifndef NDEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

        D3D11CreateDeviceAndSwapChain(nullptr,
                                      D3D_DRIVER_TYPE_HARDWARE,
                                      nullptr,
                                      createDeviceFlags,
                                      nullptr,
                                      0,
                                      D3D11_SDK_VERSION,
                                      &scd,
                                      &m_SwapChain,
                                      &m_Device,
                                      nullptr,
                                      &m_Context);
    }

    Graphics::~Graphics()
    {
        m_SwapChain->SetFullscreenState(FALSE, nullptr);
    }

    void Graphics::OnResize(float width, float height)
    {
        if (m_SwapChain)
        {
            m_Context->OMSetRenderTargets(0, nullptr, nullptr);
            m_Target->Release();
            m_SwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

            InitBackBuffer();
            SetViewport(width, height);
        }
    }

    void Graphics::InitBackBuffer()
    {
        wrl::ComPtr<ID3D11Texture2D> pBackBuffer;
        m_SwapChain->GetBuffer(0,
                               __uuidof(ID3D11Texture2D),
                               &pBackBuffer);

        m_Device->CreateRenderTargetView(pBackBuffer.Get(),
                                         nullptr,
                                         m_Target.GetAddressOf());
    }

    void Graphics::SetViewport(float width, float height)
    {
        D3D11_VIEWPORT viewport = { 0, 0, width, height };
        m_Context->RSSetViewports(1, &viewport);
    }

    void Graphics::ClearBackBuffer(float red, float green, float blue)
    {
        const float color[] = { red, green, blue, 1.0f };
        m_Context->ClearRenderTargetView(m_Target.Get(), color);
    }

    void Graphics::BeginFrame()
    {
        m_Context->OMSetRenderTargets(1, m_Target.GetAddressOf(), nullptr);
    }

    void Graphics::EndFrame()
    {
        m_SwapChain->Present(0, 0);
    }

    void Graphics::DrawTriangle()
    {
        // Initialise shaders

        wrl::ComPtr<ID3DBlob> vsBlob;
        wrl::ComPtr<ID3DBlob> psBlob;

        wrl::ComPtr<ID3D11VertexShader> vs;
        wrl::ComPtr<ID3D11PixelShader> ps;

        D3DReadFileToBlob(L"Test_VS.cso", &vsBlob);
        D3DReadFileToBlob(L"Test_PS.cso", &psBlob);

        m_Device->CreateVertexShader(vsBlob->GetBufferPointer(),
                                     vsBlob->GetBufferSize(),
                                     nullptr,
                                     &vs);

        m_Device->CreatePixelShader(psBlob->GetBufferPointer(),
                                    psBlob->GetBufferSize(),
                                    nullptr,
                                    &ps);

        m_Context->VSSetShader(vs.Get(), nullptr, 0);
        m_Context->PSSetShader(ps.Get(), nullptr, 0);

        // Define input layout

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

        wrl::ComPtr<ID3D11InputLayout> inputLayout;

        m_Device->CreateInputLayout(ied,
                                    2,
                                    vsBlob->GetBufferPointer(),
                                    vsBlob->GetBufferSize(),
                                    &inputLayout);

        m_Context->IASetInputLayout(inputLayout.Get());

        // Define vertex resources

        struct VERTEX2D
        {
            dx::XMFLOAT2 Pos;
            dx::XMFLOAT3 Color;
        };

        VERTEX2D triangle[] =
        {
            { dx::XMFLOAT2( 0.0f,  0.5f), dx::XMFLOAT3(1.0f, 0.0f, 0.0f) },
            { dx::XMFLOAT2( 0.5f, -0.5f), dx::XMFLOAT3(0.0f, 1.0f, 0.0f) },
            { dx::XMFLOAT2(-0.5f, -0.5f), dx::XMFLOAT3(0.0f, 0.0f, 1.0f) }
        };

        // Initialise Vertex Buffer

        D3D11_BUFFER_DESC vbd = { };
        vbd.Usage = D3D11_USAGE_DYNAMIC;
        vbd.ByteWidth = sizeof(triangle);
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        wrl::ComPtr<ID3D11Buffer> vertexBuffer;
        m_Device->CreateBuffer(&vbd, nullptr, &vertexBuffer);

        D3D11_MAPPED_SUBRESOURCE vmsr = { };
        m_Context->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &vmsr);
        memcpy(vmsr.pData, triangle, sizeof(triangle));
        m_Context->Unmap(vertexBuffer.Get(), 0);

        // Bind Vertex Buffer to IA stage

        const UINT stride = sizeof(VERTEX2D);
        const UINT offset = 0;

        m_Context->IASetVertexBuffers(0,
                                      1,
                                      vertexBuffer.GetAddressOf(),
                                      &stride,
                                      &offset);

        // Define indices

        const unsigned short indices[] = { 0, 1, 2 };

        // Initialise Index Buffer

        D3D11_BUFFER_DESC ibd = { };
        ibd.Usage = D3D11_USAGE_DYNAMIC;
        ibd.ByteWidth = sizeof(indices);
        ibd.StructureByteStride = sizeof(unsigned short);
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        wrl::ComPtr<ID3D11Buffer> indexBuffer;
        m_Device->CreateBuffer(&ibd, nullptr, &indexBuffer);

        D3D11_MAPPED_SUBRESOURCE imsr = { };
        m_Context->Map(indexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &imsr);
        memcpy(imsr.pData, indices, sizeof(indices));
        m_Context->Unmap(indexBuffer.Get(), 0);

        // Bind Index Buffer to IA stage

        m_Context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

        // Define constant buffer

        struct ConstBuffer
        {
            dx::XMMATRIX Transform;
        };

        UINT vpCount = 1;
        D3D11_VIEWPORT vp = { };
        m_Context->RSGetViewports(&vpCount, &vp);

        const ConstBuffer cb = 
        { 
            dx::XMMatrixTranspose(dx::XMMatrixScaling(vp.Height / vp.Width, 1.0f, 1.0f))
        };

        // Initialise Constant Buffer

        D3D11_BUFFER_DESC cbd = { };
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbd.ByteWidth = sizeof(cb);
        cbd.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA csd = { };
        csd.pSysMem = &cb;

        wrl::ComPtr<ID3D11Buffer> constantBuffer;
        m_Device->CreateBuffer(&cbd, &csd, &constantBuffer);

        // Bind Constant Buffer to VS stage

        m_Context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

        // Describe topology

        m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Finally

        m_Context->DrawIndexed(static_cast<UINT>(std::size(indices)), 0, 0);
    }

}
