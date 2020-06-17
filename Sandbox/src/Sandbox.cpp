#include "gspch.h"
#include "Sandbox/Window.h"
#include "Sandbox/Clock.h"

int main(int argc, char *argv[])
{
    return wWinMain(GetModuleHandle(nullptr), nullptr, GetCommandLine(), SW_SHOWDEFAULT);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Window window;
    window.ShowWindow(nCmdShow);

    while (Window::ProcessMessages())
    {
        window.OnUpdate();
    }

    return 0;
}
