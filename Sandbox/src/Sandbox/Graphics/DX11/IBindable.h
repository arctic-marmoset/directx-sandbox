#pragma once

namespace DX11
{
    class Graphics;
}

class IBindable
{
public:
    IBindable(const IBindable &) = delete;
    virtual ~IBindable() = default;
    virtual void Bind() const = 0;

    IBindable &operator=(const IBindable &) = delete;

protected:
    explicit IBindable(DX11::Graphics &gfx) : m_Graphics(gfx) { }

protected:
    std::reference_wrapper<DX11::Graphics> m_Graphics;
};
