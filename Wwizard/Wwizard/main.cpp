#include "Application.h"    

/*
* TODO:
* - after restart pc first open of app doesnt connect to wwise 
* - implement sort originals 
* - create Default layout
* - implement right click options for certain stuff
*/

int main()
{
    std::unique_ptr<Application> app(new Application());
    app->Loop();
    return 0;
}