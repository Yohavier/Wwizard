#include "Application.h"    
#include <WinUser.h>

int main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    std::unique_ptr<Application> app(new Application());
    app->Loop();
    return 0;
}