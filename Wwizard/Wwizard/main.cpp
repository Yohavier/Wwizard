#pragma once
#include "Application.h"    
#include <WinUser.h>

int main()
{
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
    std::unique_ptr<Application> app(new Application());
    return 0;
}