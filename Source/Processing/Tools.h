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
#include <string>

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

struct MinMaxRange
{
public:
    MinMaxRange(float min, float max) : min(min), max(max) {};
    void setMin(float min) { this->min = min; };
    void setMax(float max) { this->max = max; };
    float getMin() { return min; };
    float getMax() { return max; };
    float mapFrom0to1(float value) { return (value * (max-min) ) + min; };
private:
    float min, max;
};

string floatToString(float number, unsigned decimalPlaces);

uint64_t factorial(int x);

uint64_t nCombinations(int n, int k);

}
// debug assert
#endif /* Tools_h */
