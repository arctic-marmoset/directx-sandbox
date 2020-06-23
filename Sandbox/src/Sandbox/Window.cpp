#include "gspch.h"
#include "Window.h"

Window::Window(const Properties &props) 
    :
    m_Data(props)
{
    // Get bounding rect dimensions based on desired client dimensions
    RECT wr = { 0, 0, m_Data.Width, m_Data.Height };
    ::AdjustWindowRect(&wr, WindowTraits::GetWndStyle(0), FALSE);
    wr = { 0, 0, wr.right - wr.left, wr.bottom - wr.top };

    Create(nullptr, &wr, m_Data.Title.c_str());
}

LRESULT Window::OnResize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    const auto width = LOWORD(lParam);
    const auto height = HIWORD(lParam);
    
    // Update window properties
    m_Data.Width = width;
    m_Data.Height = height;

    // Send event to EventCallback
    WindowSizeChangedEvent event(width, height);
    m_Data.EventCallback(event);
    
    return 1; // Return 1 to signify that WM_SIZE has been handled
}

LRESULT Window::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    WindowClosedEvent event;
    m_Data.EventCallback(event);

    ::PostQuitMessage(0);
    return 0; // Return 0 to signify that WM_DESTROY has been handled
}

void Window::ProcessMessages()
{
    MSG msg = { };
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}
