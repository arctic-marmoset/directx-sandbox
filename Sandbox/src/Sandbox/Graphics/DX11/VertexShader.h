#pragma once

#include "Sandbox/Graphics/DX11/IBindable.h"

class VertexShader : public IBindable
{
public:
    VertexShader(DX11::Graphics &gfx, std::wstring fileName);
    ~VertexShader() = default;

    void Bind() const override;
    ID3DBlob *GetByteCode() const { return m_Blob.Get(); }

private:
    std::wstring m_Name;
    Microsoft::WRL::ComPtr<ID3DBlob> m_Blob;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
};
