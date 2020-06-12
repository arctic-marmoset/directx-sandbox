#include "gspch.h"
#include "Sandbox/Window.h"

int main(int argc, char *argv[])
{
    Window window;
    window.ShowWindow(SW_SHOWDEFAULT);

    while (Window::ProcessMessages())
    {
        window.OnUpdate();
    }

    return 0;
}
