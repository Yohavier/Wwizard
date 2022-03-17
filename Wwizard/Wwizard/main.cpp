#include "Application.h"    

/*
* TODO:
* - after restart pc first open of app doesnt connect to wwise 
* - opening wwise, connecting app, close wwise -> app fails to reconnect
* - reconnect button
* - reflect color coding from wwise in query results
* - implement sort originals 
*/

int main()
{
    std::unique_ptr<Application> app(new Application());
    app->Loop();
    return 0;
}