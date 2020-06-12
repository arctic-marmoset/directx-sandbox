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

    DX11::Graphics graphics(window);
    graphics.SetViewport(kCanvasWidth, kCanvasHeight);

    MSG msg = { };

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            graphics.BeginFrame();
            graphics.ClearBackBuffer(0.1f, 0.2f, 0.4f);
            graphics.DrawTriangle();
            graphics.EndFrame();
        }
    }

    return static_cast<int>(msg.wParam);
}
