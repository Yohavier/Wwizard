#include "Application.h"    
#include <WinUser.h>

/* 
* Bugs & Todos:
* - Query save Data is sometimes deleted
* 
* Toolbox Ideas
* - reset faders of selected objects
* - Enable streaming for longer files 
* 
* Add to QueryEditor:
* - open and import wav file to reaper shortcut select file and ctrl+r
* - multiselect
* 
* Add to Sort Originals:
* - count music and sfx originals
*/

int main()
{
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
    std::unique_ptr<Application> app(new Application());
    return 0;
}