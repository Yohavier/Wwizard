#include "Application.h"    

int main()
{
    std::unique_ptr<Application> app(new Application());
    app->Loop();
    return 0;
}