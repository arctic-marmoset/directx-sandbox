#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include <atlwin.h>

struct IWindowEventDelegate
{
    virtual void OnWindowSize(std::uint16_t width, std::uint16_t height) = 0;
    virtual void OnWindowClose() = 0;

    virtual ~IWindowEventDelegate() = default;
};

using WindowTraits = CWinTraits<WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW>;

class Window final : public CWindowImpl<Window, CWindow, WindowTraits>
{
public:
    DECLARE_WND_CLASS(L"GLASS_WINDOW")

    BEGIN_MSG_MAP(Window)
        MESSAGE_HANDLER(WM_SIZE,  OnResize)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)
    END_MSG_MAP()

    explicit Window(std::uint16_t width, std::uint16_t height, std::wstring title);

    std::uint16_t GetWidth() const
    {
        return m_Width;
    }

    std::uint16_t GetHeight() const
    {
        return m_Height;
    }

    HWND GetHandle() const
    {
        return m_hWnd;
    }

    IWindowEventDelegate &GetEventDelegate()
    {
        return m_EventDelegate;
    }

    void SetEventDelegate(IWindowEventDelegate &delegate)
    {
        m_EventDelegate = delegate;
    }
    
    LRESULT OnResize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    static void ProcessMessages();

private:
    inline static struct : IWindowEventDelegate
    {
        void OnWindowSize(std::uint16_t width, std::uint16_t height) override
        {
        }

        void OnWindowClose() override
        {
        }
    } s_NopEventDelegate;

    std::uint16_t   m_Width;
    std::uint16_t   m_Height;
    std::wstring    m_Title;
    std::reference_wrapper<IWindowEventDelegate> m_EventDelegate{ s_NopEventDelegate };
};
