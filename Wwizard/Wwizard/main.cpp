#include "Application.h"    

/*
* TODO:
* - opening wwise, connecting app, close wwise -> app fails to reconnect
* - create connection icon (green/red)
* - reconnect button
* - reflect color coding from wwise in query results
* - delete queries
* - implement sort originals 
*/

int main()
{
    std::unique_ptr<Application> app(new Application());
    app->Loop();
    return 0;
}