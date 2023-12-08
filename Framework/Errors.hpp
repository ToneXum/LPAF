#pragma once
#include <unordered_map>


namespace in
{
    // I should probably but this in a well formated file
    std::unordered_map<int, const char*> errors =
    {
        { 0, "The operation went smoothly." }, // anxiety
        { 1, "" }, // reserved
        { 2, "The framework is not initialised" }, // tsd::Initialise was not called
        { 3, "Invalid parameter data"}, // general missuse
        { 4, "Invalid window handle"},
        { 5, "Invalid Icon Resource" }, // tsd::Initialise
        { 6, "Invalid Cursor Resource" } // tsd::Initialise
    };
}