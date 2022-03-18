#include "Application.h"    

/*
* TODO:
* - Factory wwise queries throw exception
* - Release build doesnt run
* - after restart pc first open of app doesnt connect to wwise 
* - implement sort originals 
*/

int main()
{
    std::unique_ptr<Application> app(new Application());
    app->Loop();
    return 0;
}