#include "gsspch.hpp"
#include "Window.hpp"

Window::Window(WORD width, WORD height, std::wstring title)
    :
    m_Width(width),
    m_Height(height),
    m_Title(std::move(title))
{
    RECT wr = {
        .left   = 0,
        .top    = 0,
        .right  = m_Width,
        .bottom = m_Height,
    };

    AdjustWindowRectEx(&wr, GetWndStyle(0), FALSE, GetWndExStyle(0));

    wr = {
        .left   = 0,
        .top    = 0,
        .right  = wr.right - wr.left,
        .bottom = wr.bottom - wr.top,
    };

    Create(nullptr, &wr, m_Title.c_str());
}

LRESULT Window::OnResize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    const WORD width  = LOWORD(lParam);
    const WORD height = HIWORD(lParam);
    
    m_Width  = width;
    m_Height = height;
    
    GetEventDelegate().OnWindowSize(width, height);
    
    return 0;
}

LRESULT Window::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    GetEventDelegate().OnWindowClose();

    DestroyWindow();
    return 0;
}

void Window::ProcessMessages()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
