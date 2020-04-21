/*
  ==============================================================================

    Tools.h
    Created: 6 Apr 2020 11:48:54pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include <vector>
#include <cassert>

namespace Tools
{

using namespace std;

template<typename T>
void zeroVector(vector<T>& input)
{
    fill(input.begin(), input.end(), 0);
}

template<typename T>
void zeroVector(vector<vector<T>>& input)
{
    for(auto & i : input)
    {
        fill(i.begin(), i.end(), 0);
    }
}

template<typename T>
float toRadians(T degs)
{
    return (degs*3.14159265) / 180.f;
}

}
// debug assert
