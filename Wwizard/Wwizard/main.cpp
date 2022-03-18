#include "Application.h"    

/*
* TODO:
* - after restart pc first open of app doesnt connect to wwise 
* - opening wwise, connecting app, close wwise -> app fails to reconnect
* - reconnect button
* - implement sort originals 
* - closing wwise console properly
*/

int main()
{
    std::unique_ptr<Application> app(new Application());
    app->Loop();
    return 0;
}