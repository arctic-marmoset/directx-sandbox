#include "gspch.h"
#include "VertexShader.h"
#include "Sandbox/Graphics/DX11/Graphics.h"

VertexShader::VertexShader(DX11::Graphics &gfx, std::wstring fileName)
    :
    IBindable(gfx),
    m_Name(std::move(fileName))
{
    D3DReadFileToBlob(m_Name.c_str(), &m_Blob);

    auto *device = m_Graphics.get().GetD3DDevice();

    device->CreateVertexShader(m_Blob->GetBufferPointer(),
                               m_Blob->GetBufferSize(),
                               nullptr,
                               &m_VertexShader);
}

void VertexShader::Bind() const
{
    m_Graphics.get().GetD3DContext()->VSSetShader(m_VertexShader.Get(), nullptr, 0);
}
