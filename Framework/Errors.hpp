#pragma once
#include <unordered_map>

namespace e
{
    std::unordered_map<int, const char*> errors =
    {
        { 0, "The operation went smoothly." },
        { 1, "The criteria did not match any of the Window Instances" }
    };
}