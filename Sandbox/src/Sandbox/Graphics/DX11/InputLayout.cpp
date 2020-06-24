#include "gspch.h"
#include "InputLayout.h"
#include "Sandbox/Graphics/DX11/Graphics.h"
#include "Sandbox/Graphics/DX11/VertexShader.h"

InputLayout::InputLayout(const DX11::Graphics &gfx,
                         const VertexShader &vertexShader,
                         const D3D11_INPUT_ELEMENT_DESC *layoutDesc, 
                         std::size_t layoutDescNumElements)
    :
    m_LayoutDesc(layoutDesc, layoutDesc + layoutDescNumElements)
{
    m_Graphics = &gfx;

    const auto device = m_Graphics->GetD3DDevice();
    const auto byteCode = vertexShader.GetByteCode();

    // If no layoutDesc was provided, assume basic vertex layout
    if (!layoutDesc)
    {
        const D3D11_INPUT_ELEMENT_DESC basicVertexLayoutDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        device->CreateInputLayout(basicVertexLayoutDesc,
                                  std::size(basicVertexLayoutDesc),
                                  byteCode->GetBufferPointer(),
                                  byteCode->GetBufferSize(),
                                  &m_InputLayout);
    }
    else
    {
        device->CreateInputLayout(m_LayoutDesc.data(),
                                  m_LayoutDesc.size(),
                                  byteCode->GetBufferPointer(),
                                  byteCode->GetBufferSize(),
                                  &m_InputLayout);
    }
}

void InputLayout::Bind() const
{
    m_Graphics->GetD3DContext()->IASetInputLayout(m_InputLayout.Get());
}
