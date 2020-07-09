#include "gspch.h"
#include "PixelShader.h"
#include "Sandbox/Graphics/DX11/Graphics.h"

PixelShader::PixelShader(DX11::Graphics &gfx, std::wstring fileName)
    :
    IBindable(gfx),
    m_Name(std::move(fileName))
{
    D3DReadFileToBlob(m_Name.c_str(), &m_Blob);

    auto *device = m_Graphics.get().GetD3DDevice();

    device->CreatePixelShader(m_Blob->GetBufferPointer(),
                              m_Blob->GetBufferSize(),
                              nullptr,
                              &m_PixelShader);
}

void PixelShader::Bind() const
{
    m_Graphics.get().GetD3DContext()->PSSetShader(m_PixelShader.Get(), nullptr, 0);
}
