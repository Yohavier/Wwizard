#include "Application.h"    
#include <WinUser.h>

/*
* delete invalid/empty events
* reset faders of selected objects
* Enable streaming for longer files 
*/

int main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    std::unique_ptr<Application> app(new Application());
    app->Loop();
    return 0;
}