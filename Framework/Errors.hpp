#pragma once
#include <unordered_map>

// I really dont care about performance here so yes
namespace in
{
    // I should probably but this in a well formated file
    std::unordered_map<int, const char*> errors =
    {
        { 0, "The operation went smoothly." }, // anxiety
        { 1, "The criteria did not match any of the Window Instances" }, // caused by GetWindow
        { 2, "The framework is not initialised" }, // tsd::Initialise was not called
        { 3, "Invalid parameter data"}, // general missuse
        { 4, "This window is not valid anymore because it was closed"} // instance of closed window was accessed
    };
}