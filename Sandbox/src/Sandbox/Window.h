#pragma once

using WindowTraits = CWinTraits<WS_OVERLAPPEDWINDOW>;

class Window final : public CWindowImpl<Window, CWindow, WindowTraits>
{
public:
    // Passing -1 as background param to get (HBRUSH)0
    DECLARE_WND_CLASS_EX2(L"WindowClass", Window, CS_OWNDC, -1)

    BEGIN_MSG_MAP(Window)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    END_MSG_MAP()

    Window(unsigned int width, unsigned int height);
    Window(const Window &) = delete;
    
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

    Window &operator=(const Window &) = delete;
};
