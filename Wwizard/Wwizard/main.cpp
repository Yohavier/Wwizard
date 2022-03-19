#include "Application.h"    

/*
* TODO:
* - Release build doesnt run
* - after restart pc first open of app doesnt connect to wwise 
* - implement sort originals 
* - create Default layout
* - editing waql queries is not right 
* - waql query editing always adds characters
*/

int main()
{
    std::unique_ptr<Application> app(new Application());
    app->Loop();
    return 0;
}