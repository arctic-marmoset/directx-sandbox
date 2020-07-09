#pragma once

#include "Sandbox/Graphics/DX11/IBindable.h"

class PixelShader : public IBindable
{
public:
    PixelShader(DX11::Graphics &gfx, std::wstring fileName);
    ~PixelShader() = default;

    void Bind() const override;
    ID3DBlob *GetByteCode() const { return m_Blob.Get(); }

private:
    std::wstring m_Name;
    Microsoft::WRL::ComPtr<ID3DBlob> m_Blob;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
};
