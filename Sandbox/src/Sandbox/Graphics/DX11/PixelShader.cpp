#include "gspch.h"
#include "PixelShader.h"
#include "Sandbox/Graphics/DX11/Graphics.h"

PixelShader::PixelShader(const DX11::Graphics &gfx, std::wstring fileName)
    :
    m_Name(std::move(fileName))
{
    m_Graphics = &gfx;
    D3DReadFileToBlob(m_Name.c_str(), &m_Blob);

    auto device = m_Graphics->GetD3DDevice();
    auto context = m_Graphics->GetD3DContext();

    device->CreatePixelShader(m_Blob->GetBufferPointer(),
                              m_Blob->GetBufferSize(),
                              nullptr,
                              &m_PixelShader);
}

void PixelShader::Bind() const
{
    m_Graphics->GetD3DContext()->PSSetShader(m_PixelShader.Get(), nullptr, 0);
}
