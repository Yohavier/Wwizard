#include "Application.h"    

/*
* TODO:
* - after restart pc first open of app doesnt connect to wwise 
* - implement sort originals 
*/

int main()
{
    std::unique_ptr<Application> app(new Application());
    app->Loop();
    return 0;
}