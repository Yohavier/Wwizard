#include "Application.h"    

/*
* - implement sort originals 
* - create Default layout
* - reconnecting breaks query editor
*/

int main()
{
    std::unique_ptr<Application> app(new Application());
    app->Loop();
    return 0;
}