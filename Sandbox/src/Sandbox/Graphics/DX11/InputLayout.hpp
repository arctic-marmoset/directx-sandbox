#pragma once

#include "Sandbox/Graphics/DX11/IBindable.hpp"

class VertexShader;

class InputLayout : public IBindable
{
public:
    InputLayout(
        DX11::Graphics                 &gfx,
        const VertexShader             &vertexShader,
        const D3D11_INPUT_ELEMENT_DESC *layoutDesc = nullptr,
        std::size_t                     layoutDescNumElements = 0
    );

    void Bind() const override;

private:
    std::vector<D3D11_INPUT_ELEMENT_DESC>     m_LayoutDesc;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
};
