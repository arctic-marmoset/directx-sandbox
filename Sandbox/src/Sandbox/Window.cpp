#include "gspch.h"
#include "Window.h"

Window::Window(const Properties &props) 
    :
    m_Data(props)
{
    RECT wr = { 0, 0, m_Data.Width, m_Data.Height };
    ::AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    wr = { 0, 0, wr.right - wr.left, wr.bottom - wr.top };

    Create(nullptr, &wr, m_Data.Title.c_str(), WS_OVERLAPPEDWINDOW);

    m_Graphics = std::make_unique<DX11::Graphics>(m_hWnd);
    m_Graphics->SetViewport(static_cast<float>(m_Data.Width), 
                            static_cast<float>(m_Data.Height));
}

void Window::OnUpdate()
{
    m_Graphics->BeginFrame();
    m_Graphics->ClearBackBuffer(0.1f, 0.2f, 0.4f);
    m_Graphics->DrawTriangle();
    m_Graphics->EndFrame();
}

LRESULT Window::OnResize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    const auto width = LOWORD(lParam);
    const auto height = HIWORD(lParam);
    m_Graphics->OnResize(width, height);
    return 1;
}

LRESULT Window::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    ::PostQuitMessage(0);
    return 0;
}

bool Window::ProcessMessages()
{
    MSG msg = { };
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            return false;
        }

        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    return true;
}
