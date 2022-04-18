#include "Application.h"    
#include <WinUser.h>

/* 
* Bugs & Technical Debt:
* - Query save Data is sometimes deleted
* - CheckSeparator extract out out add last layer function
* - Physical folders currently dont get checked for naming
* 
* Toolbox Ideas
* - delete invalid/empty events
* - reset faders of selected objects
* - Enable streaming for longer files 
* - find large files
* 
* Add QueryEditor:
* - open and import wav file to reaper shortcut select file and ctrl+r
* - multiselect
* 
* Add Sort Originals:
* - count music and sfx originals
*/

int main()
{
    //ShowWindow(GetConsoleWindow(), SW_HIDE);
    std::unique_ptr<Application> app(new Application());
    return 0;
}