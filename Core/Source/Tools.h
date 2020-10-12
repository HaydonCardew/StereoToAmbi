/*
  ==============================================================================

    Tools.h
    Created: 6 Apr 2020 11:48:54pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#ifndef Tools_h
#define Tools_h

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
unsigned numberOfNonZeros(const vector<T>& input)
{
    unsigned nnz = 0;
    for(auto & i : input)
    {
        if(i != 0)
        {
            nnz++;
        }
    }
    return nnz;
}

template<typename T>
float toRadians(T degs)
{
    return (degs*3.14159265) / 180.f;
}

uint64_t factorial(int x);

int nCombinations(int n, int k);

}
// debug assert
#endif /* Tools_h */
