#ifndef Histogram_h
#define Histogram_h

#include <stdio.h>
#include <vector>

using namespace std;

class Histogram
{
public:
    Histogram(unsigned nBins);
    Histogram(unsigned nBins, unsigned nThresholds);
    Histogram(vector<int> bins); // loading bins directly
    void loadData(const vector<float>& data, unsigned start, unsigned end);
    void loadData(const vector<float>& data);
    void setupMultiLevelThreshold(unsigned nThresholds);
    float getMaxValue();
    vector<unsigned> getThresholdBins();
    vector<float> getThresholdValues();
    
private:
    void calculateProbabilityBins();
    unsigned nBins;
    vector<int> bins;
    vector<float> probabilityBins;
    vector<float> weightedProbabilityBins;
    float increment;
    float maxValue;
    unsigned nThresholds;
    void createIndicesLookupTable(int nBins, int nThresholds);
    vector<vector<unsigned>> lookupIndices;
    vector<vector<float>> p;
    vector<vector<float>> s;
};

#endif /* Histogram_h */
