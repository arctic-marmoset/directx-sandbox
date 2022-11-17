#include "gsspch.hpp"
#include "Application.hpp"

Application::Application()
    :
    m_Window(1280U, 720U, L"DirectX Renderer"),
    m_VertexShader(m_Graphics, L"Phong_VS.cso"),
    m_PixelShader(m_Graphics, L"Phong_PS.cso"),
    m_InputLayout(m_Graphics, m_VertexShader)
{
    m_Window.SetEventDelegate(*this);
    m_Graphics.SetWindow(m_Window);

    InitGraphicsResources();
}

void Application::InitGraphicsResources()
{
    m_VertexShader.Bind();
    m_PixelShader.Bind();
    m_InputLayout.Bind();
}

int Application::Run()
{
    m_Window.ShowWindow(SW_SHOWDEFAULT);

    while (m_IsRunning)
    {
        m_Clock.Tick();
        Window::ProcessMessages();
        m_Graphics.BeginFrame();
        m_Graphics.Clear(0.1F, 0.1F, 0.1F);
        m_Graphics.DrawCube(m_Clock.TimeTotal());
        m_Graphics.EndFrame();
    }

    return EXIT_SUCCESS;
}

void Application::OnWindowSize(std::uint16_t width, std::uint16_t height)
{
    m_Graphics.SetDimensions(width, height);
}

void Application::OnWindowClose()
{
    m_IsRunning = false;
}
