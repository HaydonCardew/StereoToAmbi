/*
  ==============================================================================

    Tools.cpp
    Created: 8 Oct 2020 12:15:30am
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "Tools.h"

uint64_t Tools::factorial(int x)
{
    uint64_t val = x--;
    while (x > 1)
    {
        val *= x--;
    }
    return val;
}

int Tools::nCombinations(int n, int k)
{
    return factorial(n) / (factorial(n - k) * factorial(k));
}
