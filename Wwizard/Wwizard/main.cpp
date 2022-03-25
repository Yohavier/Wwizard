#include "Application.h"    

/*
* Main
* - create Default layout
* - reconnecting breaks query editor
* - after restart pc first open of app doesnt connect to wwise
* 
* Query Module:
* - Display container type of result
* 
* Sort Originals:
* count all wav that are in originals
* remove unused wav
* define which containers can create subfolders [checkboxes]
*/

int main()
{
    std::unique_ptr<Application> app(new Application());
    app->Loop();
    return 0;
}