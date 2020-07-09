#include "gspch.h"
#include "Application.h"

#define BIND_HANDLER_FN(x) std::bind(&x, this, std::placeholders::_1)

Application::Application()
    :
    m_WindowClosed(false)
{
    m_Window.SetEventCallback(BIND_HANDLER_FN(Application::OnEvent));
    m_Graphics = std::make_unique<DX11::Graphics>();
    m_Graphics->SetWindow(m_Window);
    m_Window.ShowWindow(SW_SHOWDEFAULT);

    InitGraphicsResources();
}

Application::~Application()
{
}

void Application::InitGraphicsResources()
{
    m_VertexShader = std::make_unique<VertexShader>(*m_Graphics, L"Phong_VS.cso");
    m_PixelShader  = std::make_unique<PixelShader>(*m_Graphics, L"Phong_PS.cso");
    m_InputLayout  = std::make_unique<InputLayout>(*m_Graphics, *m_VertexShader);

    m_VertexShader->Bind();
    m_PixelShader->Bind();
    m_InputLayout->Bind();
}

void Application::Run()
{
    while (!m_WindowClosed)
    {
        m_Clock.Tick();
        Window::ProcessMessages();
        m_Graphics->BeginFrame();
        m_Graphics->Clear(0.1f, 0.1f, 0.1f);
        m_Graphics->DrawCube(m_Clock.TimeTotal());
        m_Graphics->EndFrame();
    }
}

void Application::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowSizeChangedEvent>(BIND_HANDLER_FN(Application::OnWindowSizeChanged));
    dispatcher.Dispatch<WindowClosedEvent>(BIND_HANDLER_FN(Application::OnWindowClosed));
}

bool Application::OnWindowSizeChanged(WindowSizeChangedEvent &e)
{
    m_Graphics->SetDimensions(e.GetWidth(), e.GetHeight());
    return true;
}

bool Application::OnWindowClosed(WindowClosedEvent &e)
{
    m_WindowClosed = true;
    return true;
}
