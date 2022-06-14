#pragma once
#include <string> 
#include <set>
#include <Windows.h>

static const std::string GenerateGuid()
{
    return std::to_string(((long long)rand() << 32) | rand());
}

static bool ContainsSpecialCharacters(const std::string& check)
{
    char specialCha[9] = {'/', '\\', ':', '* ', '?', '|', '<', '>', '"'};
    
    for (const auto& ch : check)
    {
        for (const auto& sc : specialCha)
        {
            if (ch == sc)
            {
                return true;
            }
        }
    }
    return false;
}

static void OpenURL(const std::string url)
{
    ShellExecute(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}