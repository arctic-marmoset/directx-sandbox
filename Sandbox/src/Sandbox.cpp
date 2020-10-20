#include "gspch.h"
#include "Sandbox/Application.h"

int wmain(int argc, wchar_t *argv[])
{
#ifdef NDEBUG
    FreeConsole();
#endif
    Application app;
    app.Run();
}
