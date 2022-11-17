#include "gsspch.hpp"

#include "Sandbox/Application.hpp"

#ifndef NDEBUG
#include <crtdbg.h>
#endif

int APIENTRY wWinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     LPWSTR    lpCmdLine,
    _In_     int       nShowCmd
)
{
#ifndef NDEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        Application app;
        app.Run();

        return EXIT_SUCCESS;
    }
    catch (const std::exception &exception)
    {
        const std::string message = std::format("fatal error: {}\n", exception.what());
        OutputDebugStringA(message.c_str());
    }
    catch (...)
    {
        OutputDebugString(L"fatal error: an unknown exception has occurred\n");
    }

    MessageBox(
        nullptr,
        L"A fatal error has occurred. The application will now close.",
        L"Fatal Error",
        MB_ICONERROR | MB_OK
    );

    return EXIT_FAILURE;
}
