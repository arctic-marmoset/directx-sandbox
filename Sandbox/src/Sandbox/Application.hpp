#pragma once

#include "Sandbox/Clock.hpp"
#include "Sandbox/Window.hpp"
#include "Sandbox/Graphics/DX11/Graphics.hpp"
#include "Sandbox/Graphics/DX11/VertexShader.hpp"
#include "Sandbox/Graphics/DX11/PixelShader.hpp"
#include "Sandbox/Graphics/DX11/InputLayout.hpp"

class Application : public IWindowEventDelegate
{
public:
    Application();

    int Run();

    void OnWindowSize(std::uint16_t width, std::uint16_t height) override;
    void OnWindowClose() override;

private:
    void InitGraphicsResources();

    bool m_IsRunning{ true };

    Clock m_Clock;

    Window m_Window;

    DX11::Graphics m_Graphics;

    VertexShader m_VertexShader;
    PixelShader  m_PixelShader;
    InputLayout  m_InputLayout;
};
