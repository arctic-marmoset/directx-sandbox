#include "gspch.h"
#include "Sandbox/Application.h"

int main(int argc, char *argv[])
{
    return wWinMain(GetModuleHandle(nullptr), nullptr, GetCommandLine(), SW_SHOWDEFAULT);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Application app;
    app.Run();
    return 0;
}
