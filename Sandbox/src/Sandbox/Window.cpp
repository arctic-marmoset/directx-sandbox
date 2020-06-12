#include "gspch.h"
#include "Window.h"

Window::Window(unsigned int width, unsigned int height)
{
    RECT wr = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    wr = { 0, 0, wr.right - wr.left, wr.bottom - wr.top };

    Create(nullptr, &wr, L"DirectX Program", WS_OVERLAPPEDWINDOW);
}

LRESULT Window::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    ::PostQuitMessage(0);
    return 0;
}
