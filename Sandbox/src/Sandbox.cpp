#include "gspch.h"
#include "Sandbox/Window.h"
#include "Sandbox/Graphics/DX11/Graphics.h"

// CONSTANTS

constexpr UINT kCanvasWidth = 1280;
constexpr UINT kCanvasHeight = 720;

int main(int argc, char *argv[])
{
    Window window(kCanvasWidth, kCanvasHeight);
    window.ShowWindow(SW_SHOWDEFAULT);

    while (Window::ProcessMessages())
    {
        window.OnUpdate();
    }

    return 0;
}
