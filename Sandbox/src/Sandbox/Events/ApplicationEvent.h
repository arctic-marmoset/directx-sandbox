#pragma once

#include "Sandbox/Events/Event.h"

class WindowSizeChangedEvent final : public Event
{
public:
    WindowSizeChangedEvent(int width, int height)
        :
        m_Width(width),
        m_Height(height)
    {
    }

    ~WindowSizeChangedEvent() = default;

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

    virtual std::wstring ToString() const override
    {
        std::wostringstream ss;
        ss << L"WindowSizeChangedEvent: (" << m_Width << ", " << m_Height << ")";
        return ss.str();
    }

    DECLARE_EVENT_CLASS_TYPE(WindowSizeChanged)

private:
    int m_Width;
    int m_Height;
};

class WindowClosedEvent : public Event
{
public:
    WindowClosedEvent()
    {
    }

    ~WindowClosedEvent() = default;

    DECLARE_EVENT_CLASS_TYPE(WindowClosed)
};
