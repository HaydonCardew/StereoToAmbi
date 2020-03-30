#include "Histogram.h"
#include <numeric>
#include <math.h>

void Histogram::loadData(vector<float>::iterator start, vector<float>::iterator end)
{
    std::fill(bins.begin(), bins.end(), 0);
    float maxValue = *std::max_element(start, end);
    float increment = maxValue / ((float)nBins - 1);
    if (maxValue == 0)
    {
        bins[0] = (unsigned)distance(start, end);
    }
    else
    {
        for (vector<float>::iterator it = start; it !=end; ++it)
        {
            // for linear
            int index = floor(*it / increment);
            // for non linear
            //float tmp = pow(panMap[channel][i] / histogram.maxValue[channel], 1) * histogram.maxValue[channel];
            //int index = floor(tmp / histogram.increment[channel]);
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
    }
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
