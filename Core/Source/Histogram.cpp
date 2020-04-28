#include "Histogram.h"
#include <numeric>
#include <math.h>
#include "Tools.hpp"
#include <iostream>

Histogram::Histogram(unsigned nBins) : nBins(nBins), increment(0.f), maxValue(0.f)
{
    bins.resize(nBins);
    probabilityBins.resize(nBins);
    weightedProbabilityBins.resize(nBins);
}

void Histogram::loadData(const vector<float>& data, unsigned start, unsigned end)
{
    assert(data.size() >= end);
    assert(end > start);
    Tools::zeroVector(bins);
    auto startIter = data.begin() + start;
    auto endIter = data.begin() + end;
    maxValue = *std::max_element(startIter, endIter);
    assert(!isinf(maxValue));
    if (maxValue == 0.f)
    {
        bins[0] = end - start;
    }
    else
    {
        increment = maxValue / float(nBins - 1);
        for(auto value = startIter; value != (endIter+1); ++value)
        {
            assert(increment > 0);
            int index = floor(*value / increment);
            if (index >= nBins)
            {
                index = nBins - 1;
            }
            else if (index < 0)
            {
                index = 0;
            }
            bins[index]++;
        }
        assert(!isinf(increment));
    }
    calculateProbabilityBins();
}

void Histogram::calculateProbabilityBins()
{
    float total = accumulate(bins.begin(), bins.end(), 0);
    for(int i = 0; i < bins.size(); ++i)
    {
        probabilityBins[i] = ((float)bins[i]) / total;
        weightedProbabilityBins[i] = probabilityBins[i] * (i+1);
    }
}

vector<float>::iterator Histogram::getFirstProbabilityBin()
{
    return probabilityBins.begin();
}

vector<float>::iterator Histogram::getLastProbabilityBin()
{
    return probabilityBins.end();
}

vector<float>::iterator Histogram::getFirstWeightedProbabilityBin()
{
    return weightedProbabilityBins.begin();
}

vector<float>::iterator Histogram::getLastWeightedProbabilityBin()
{
    return weightedProbabilityBins.end();
}

float Histogram::getMaxValue()
{
    return maxValue;
}

float Histogram::getIncrement()
{
    return increment;
}
