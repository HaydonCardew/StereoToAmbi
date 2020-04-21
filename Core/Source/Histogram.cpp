#include "Histogram.h"
#include <numeric>
#include <math.h>
#include "Tools.hpp"
#include <iostream>

Histogram::Histogram(unsigned nBins) : nBins(nBins)
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
    maxValue = *std::max_element(data.begin() + start, data.begin() + end);
    increment = maxValue / float(nBins - 1);
    int nValues = end-start;
    if (maxValue == 0)
    {
        //bins[0] = (unsigned)distance(start, end);
        bins[0] = (unsigned)nValues;
    }
    else
    {
        //for (vector<float>::iterator it = start; it !=end; ++it)
        for (int i = 0; i < nValues; ++i)
        {
            if(data[i] == 0.0)
            {
                //continue;
            }
            // for linear
            int index = floor(data[i] / increment);
            //cout << "It : " << data[i] << " Bin : " << index << endl;
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
