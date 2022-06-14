#pragma once
#include <string> 
#include <set>
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