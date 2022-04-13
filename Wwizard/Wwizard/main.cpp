#include "Application.h"    
#include <WinUser.h>

/* 
* TODO:
* delete invalid/empty events
* reset faders of selected objects
* Enable streaming for longer files 
* find large files
* open and import wav file to reaper shortcut select file and ctrl+r
* multiselect
* count music and sfx originals
* Query save Data is sometimes deleted
* reconnecting breaks naming convention execute
*/

int main()
{
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    std::unique_ptr<Application> app(new Application());
    return 0;
}