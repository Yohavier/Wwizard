#define _CRTDBG_MAP_ALLOC
#include "Application.h"    
#include <crtdbg.h>

int main(int, char**)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    std::unique_ptr<Application> app(new Application());
    app->Loop();
    return 0;
}