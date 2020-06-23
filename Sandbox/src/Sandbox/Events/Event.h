#pragma once

#define DECLARE_EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; } \
                                       virtual EventType GetEventType() const override { return GetStaticType(); } \
                                       virtual const wchar_t *GetName() const override { return L#type; }

enum class EventType
{
    Unknown = 0,
    WindowSizeChanged, WindowClosed
};

class Event
{
public:
    virtual ~Event() = default;

    virtual EventType GetEventType() const = 0;
    virtual const wchar_t *GetName() const = 0;
    virtual std::wstring ToString() const { return GetName(); }

public:
    bool Handled = false;
};

class EventDispatcher
{
    template<typename T>
    using EventHandler = std::function<bool(T &)>;

public:
    EventDispatcher(Event &event)
        :
        m_Event(event)
    {
    }

    template<typename T>
    bool Dispatch(EventHandler<T> func)
    {
        // Only dispatch to the handler if the type matches
        if (m_Event.GetEventType() == T::GetStaticType())
        {
            m_Event.Handled = func(reinterpret_cast<T &>(m_Event));
            return true;
        }
        return false;
    }

private:
    Event &m_Event;
};

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
