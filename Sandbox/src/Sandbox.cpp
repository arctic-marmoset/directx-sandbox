#include "gspch.h"

constexpr LPCWSTR kWndClassName = L"WindowClass";

LRESULT CALLBACK WindowProc(HWND windowHandle,
                            UINT message,
                            WPARAM wParam,
                            LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    HWND hWnd = nullptr;

    WNDCLASSEX wc = { };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(30, 30, 40));
    wc.lpszClassName = kWndClassName;

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(0,
                          kWndClassName,
                          L"DirectX Program",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          nullptr,
                          nullptr,
                          hInstance,
                          nullptr);

    ShowWindow(hWnd, nCmdShow);

    MSG msg = { };

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    UnregisterClass(kWndClassName, hInstance);

    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(HWND hWnd, 
                            UINT message, 
                            WPARAM wParam, 
                            LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}
