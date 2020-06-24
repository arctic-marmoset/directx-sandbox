#pragma once

namespace DX11
{
    class Graphics;
}

class IBindable
{
public:
    virtual ~IBindable() = default;
    virtual void Bind() const = 0;

protected:
    // Store as raw pointer since Graphics is owned by Application,
    // and Application lifetime should exceed IBindable lifetime.
    const DX11::Graphics *m_Graphics = nullptr;
};
