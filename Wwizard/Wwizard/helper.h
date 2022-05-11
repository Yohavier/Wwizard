#pragma once
#include <string>

static const std::string GenerateGuid()
{
    return std::to_string(((long long)rand() << 32) | rand());
}