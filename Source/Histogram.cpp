#include "Histogram.h"
#include <numeric>
#include <math.h>
#include "Tools.h"
#include <iostream>
#include <algorithm>

Histogram::Histogram(unsigned nBins) : nBins(nBins), increment(0.f), maxValue(0.f), nThresholds(0)
{
    bins.resize(nBins);
    probabilityBins.resize(nBins);
    weightedProbabilityBins.resize(nBins);
}

Histogram::Histogram(unsigned nBins, unsigned nThresholds) : nBins(nBins), increment(0.f), maxValue(0.f), nThresholds(nThresholds)
{
    bins.resize(nBins);
    probabilityBins.resize(nBins);
    weightedProbabilityBins.resize(nBins);
    setupMultiLevelThreshold(nThresholds);
}

Histogram::Histogram(vector<int> bins) : bins(bins), nThresholds(0)
{
    nBins = (unsigned)bins.size();
    probabilityBins.resize(nBins);
    weightedProbabilityBins.resize(nBins);
    calculateProbabilityBins();
    maxValue = 1; // dummy
    increment = 0.1; // dummy
}

void Histogram::loadData(const vector<float>& data)
{
    loadData(data, 0, (unsigned)data.size());
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

void Histogram::setupMultiLevelThreshold (unsigned nThresholds)
{
    this->nThresholds = nThresholds;
    createIndicesLookupTable(nBins, nThresholds);
    p.resize(nBins, vector<float>(nBins, 0));
    s.resize(nBins, vector<float>(nBins, 0));
}

void Histogram::createIndicesLookupTable(int nBins, int nThresholds)
{
    lookupIndices.clear();
    const int flag = 1;
    vector<int> mask(nThresholds, flag);
    mask.resize(nBins-1, 0); // -1 as the last index is not needed in getVarSq
    vector<unsigned> index(nThresholds, 0);
    do
    {
        auto it = find(mask.begin(), mask.end(), flag);
        for ( auto & i : index )
        {
            i = (unsigned)distance(mask.begin(), it); // - 1; // this will point to index in vector so must use -1
            it = find(next(it), mask.end(), flag);
        }
        lookupIndices.push_back(index);
    } while ( next_permutation(mask.rbegin(), mask.rend()) );
}

vector<unsigned> Histogram::getThresholdBins()
{
    partial_sum(probabilityBins.begin(), probabilityBins.end(), p[0].begin());
    assert(p[0].back() > 0.99);
    partial_sum(weightedProbabilityBins.begin(), weightedProbabilityBins.end(), s[0].begin());
    
    for (int u = 1; u < nBins; u++)
    {
        for (int v = u; v < nBins; v++)
        {
            p[u][v] = p[0][v] - p[0][u-1]; // P(u,v)= P(1,v) - P(1,u-1)
            s[u][v] = s[0][v] - s[0][u-1];
        }
    }
    
    for (int u = 0; u < nBins; u++)
    {
        for (int v = u; v < nBins; v++)
        {
            if (p[u][v] != 0)
            { //never had this issue in matlab?!
                p[u][v] = (s[u][v] * s[u][v]) / p[u][v];
            }
        }
    }

    auto getVarSq = [&](vector<unsigned>& indices)
    {
        const unsigned end = nBins - 1;
        float varSq = p[0][ indices[0] ];
        for (int i = 0; i < (nThresholds-1); ++i )
        {
            varSq += p[ indices[i] + 1 ][ indices[i+1] ];
        }
        varSq += p[ indices[nThresholds-1] + 1 ][ end ];
        return varSq;
    };
    
    float varSq = 0;
    float tmp = 0;
    int thresholdIndex = 0;
    for (int i = 0; i < lookupIndices.size(); ++i)
    {
        tmp = getVarSq(lookupIndices[i]);
        if (tmp > varSq)
        {
            varSq = tmp;
            thresholdIndex = i;
        }
    }
    return lookupIndices[thresholdIndex];
}

vector<float> Histogram::getThresholdValues()
{
    vector<unsigned> thresholds = getThresholdBins();
    vector<float> thresholdValues(thresholds.size());
    for (int i = 0; i < thresholdValues.size(); ++i)
    {
        thresholdValues[i] = (float)thresholds[i] * increment;
    }
    return thresholdValues;
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

float Histogram::getMaxValue()
{
    return maxValue;
}
