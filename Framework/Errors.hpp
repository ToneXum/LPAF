// MIT License
//
// Copyright(c) 2023 ToneXum
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <unordered_map>


namespace in
{
    // Performance is not important here so yes, have a hashmap
    std::unordered_map<int, const char*> errors =
    {
    { 0, "The operation went smoothly." }, // no error
    { 1, "The framework is already initialised." }, // tsd::Initialise was called more than once
    { 2, "The framework is not initialised." }, // tsd::Initialise was not called
    { 3, "Invalid parameter data."}, // general missuse
    { 4, "Invalid window handle."},
    { 5, "Invalid Icon Resource." }, // tsd::Initialise
    { 6, "Invalid Cursor Resource." }, // tsd::Initialise
    { 7, "65535 windows have been opened, cannot create more." } // I hope no one will have to fetch this...
    };
}