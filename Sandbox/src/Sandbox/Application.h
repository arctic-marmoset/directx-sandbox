#pragma once

#include "Sandbox/Clock.h"
#include "Sandbox/Window.h"
#include "Sandbox/Events/ApplicationEvent.h"
#include "Sandbox/Graphics/DX11/Graphics.h"
#include "Sandbox/Graphics/DX11/VertexShader.h"
#include "Sandbox/Graphics/DX11/PixelShader.h"
#include "Sandbox/Graphics/DX11/InputLayout.h"

class Application
{
public:
    Application();
    ~Application();

    void Run();

protected:
    void OnEvent(Event &e);
    bool OnWindowSizeChanged(WindowSizeChangedEvent &e);
    bool OnWindowClosed(WindowClosedEvent &e);

private:
    void InitGraphicsResources();

private:
    bool m_WindowClosed;
    Clock m_Clock;
    Window m_Window;
    std::unique_ptr<DX11::Graphics> m_Graphics;

    std::unique_ptr<InputLayout>  m_InputLayout;
    std::unique_ptr<VertexShader> m_VertexShader;
    std::unique_ptr<PixelShader>  m_PixelShader;

};
