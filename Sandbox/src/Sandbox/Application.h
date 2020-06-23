#pragma once

#include "Sandbox/Events/Event.h"
#include "Sandbox/Window.h"
#include "Sandbox/Graphics/DX11/Graphics.h"
#include "Sandbox/Clock.h"

class Application
{
public:
    Application();
    ~Application();

    void Run();

protected:
    void OnEvent(Event &e);
    bool OnWindowSizeChanged(WindowSizeChangedEvent &e);
    bool OnWindowClosed(WindowClosedEvent &e);

private:
    bool m_WindowClosed;

    Window m_Window;
    std::unique_ptr<DX11::Graphics> m_Graphics;

    Clock m_Clock;
};
