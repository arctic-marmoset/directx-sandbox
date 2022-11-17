#pragma once

namespace DX11
{
    class Graphics;
}

class IBindable
{
public:
    virtual void Bind() const = 0;

    virtual ~IBindable() = default;

    IBindable(const IBindable &) = delete;
    IBindable &operator=(const IBindable &) = delete;

protected:
    explicit IBindable(DX11::Graphics &gfx)
        :
        m_Graphics(gfx)
    {
    }

    DX11::Graphics &GetGraphics() const
    {
        return m_Graphics;
    }

private:
    DX11::Graphics &m_Graphics;
};
