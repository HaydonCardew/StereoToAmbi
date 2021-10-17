/*
  ==============================================================================

    Tools.cpp
    Created: 8 Oct 2020 12:15:30am
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "Tools.h"
#include <sstream>
#include <iostream>
#include <iomanip>

uint64_t Tools::factorial(int x)
{
    uint64_t val = x--;
    while (x > 1)
    {
        val *= x--;
    }
    return val;
}

uint64_t Tools::nCombinations(int n, int k)
{
    return factorial(n) / (factorial(n - k) * factorial(k));
}

std::string Tools::floatToString(float number, unsigned decimalPlaces)
{
    stringstream stream;
    stream << fixed << setprecision(decimalPlaces) << number;
    return stream.str();
}
