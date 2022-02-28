#include "Application.h"    

int main(int, char**)
{
    std::unique_ptr<Application> app(new Application());
    app->Loop();
   
    return 0;
}