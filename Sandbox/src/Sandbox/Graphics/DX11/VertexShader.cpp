#include "gsspch.hpp"
#include "VertexShader.hpp"

#include "Sandbox/Graphics/DX11/Graphics.hpp"

VertexShader::VertexShader(DX11::Graphics &gfx, std::wstring fileName)
    :
    IBindable(gfx),
    m_Name(std::move(fileName))
{
    D3DReadFileToBlob(m_Name.c_str(), &m_Blob);

    auto *device = GetGraphics().GetD3DDevice();

    device->CreateVertexShader(
        m_Blob->GetBufferPointer(),
        m_Blob->GetBufferSize(),
        nullptr,
        &m_VertexShader
    );
}

void VertexShader::Bind() const
{
    GetGraphics().GetD3DContext()->VSSetShader(m_VertexShader.Get(), nullptr, 0);
}
