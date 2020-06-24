#include "gspch.h"
#include "VertexShader.h"
#include "Sandbox/Graphics/DX11/Graphics.h"

VertexShader::VertexShader(const DX11::Graphics &gfx, std::wstring fileName)
    :
    m_Name(std::move(fileName))
{
    m_Graphics = &gfx;
    D3DReadFileToBlob(m_Name.c_str(), &m_Blob);

    auto device = m_Graphics->GetD3DDevice();
    auto context = m_Graphics->GetD3DContext();

    device->CreateVertexShader(m_Blob->GetBufferPointer(),
                               m_Blob->GetBufferSize(),
                               nullptr,
                               &m_VertexShader);
}

void VertexShader::Bind() const
{
    m_Graphics->GetD3DContext()->VSSetShader(m_VertexShader.Get(), nullptr, 0);
}
