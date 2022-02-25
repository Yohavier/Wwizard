#include <iostream>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx10.h"
#include <d3d10_1.h>
#include <d3d10.h>
#include <tchar.h>
#include "Dockspace.h"
#include <string>
#include <sstream>
#include <iostream>
#include "QueryHelper.h"
#include <vector>
#include "Application.h"    

#if defined( AK_ENABLE_ASSERTS )

void SampleAssertHook(const char* in_pszExpression, const char* in_pszFileName, int in_lineNumber)
{
    std::cout << 2;
}

AkAssertHook g_pAssertHook = SampleAssertHook;

#endif

void wwise()
{
    using namespace AK::WwiseAuthoringAPI;
    QueryHelper* helper = new QueryHelper();
    helper->ChangeSettings("127.0.0.1", 8080);

    AkJson args(AkJson::Map{
            { "from", AkJson::Map{
                { "path", AkJson::Array{ AkVariant("\\Queries") } } } }
        });

    AkJson options(AkJson::Map{
        { "return", AkJson::Array{ AkVariant("id"), AkVariant("name"), AkVariant("type")}}});

    std::vector<std::string> output;
    helper->WalkProject(args, options, output);

    for (auto object : output)
    {
        std::cout << object << std::endl;
    }
    delete helper;
}
int main(int, char**)
{
    std::unique_ptr<Application> app(new Application());
    app->Loop();
   
    return 0;
}