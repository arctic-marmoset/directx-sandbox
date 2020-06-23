#pragma once

#include "Sandbox/Clock.h"
#include "Sandbox/Graphics/DX11/Graphics.h"

using WindowTraits = CWinTraits<WS_OVERLAPPEDWINDOW>;

class Window final : public CWindowImpl<Window, CWindow, WindowTraits>
{
public:
    struct Properties
    {
        int Width;
        int Height;
        std::wstring Title;

        Properties(int width = 1280,
                   int height = 720,
                   const std::wstring &title = L"DirectX Program")
            :
            Width(width),
            Height(height),
            Title(title)
        {
        }
    };

public:
    // Passing -1 as background param to get (HBRUSH)0
    DECLARE_WND_CLASS_EX2(L"WindowClass", Window, 0, -1)

    BEGIN_MSG_MAP(Window)
        MESSAGE_HANDLER(WM_SIZE, OnResize)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    END_MSG_MAP()

    Window(const Properties &props = Properties());
    Window(const Window &) = delete;

    void OnUpdate();
    
    LRESULT OnResize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

    Window &operator=(const Window &) = delete;

public:
    static bool ProcessMessages();

private:
    Properties m_Data;
    std::unique_ptr<DX11::Graphics> m_Graphics;
    Clock m_Clock;
};
