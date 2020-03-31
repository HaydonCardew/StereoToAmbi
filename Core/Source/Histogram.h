#ifndef Histogram_h
#define Histogram_h

#include <stdio.h>
#include <vector>

using namespace std;

class Histogram
{
public:
    Histogram(unsigned nBins);
    void loadData(vector<float>::iterator start, vector<float>::iterator end);
    vector<float>::iterator getFirstProbabilityBin();
    vector<float>::iterator getLastProbabilityBin();
    vector<float>::iterator getFirstWeightedProbabilityBin();
    vector<float>::iterator getLastWeightedProbabilityBin();
    float getMaxValue();
    float getIncrement();
    
private:
    void calculateProbabilityBins();
    unsigned nBins;
    vector<int> bins;
    vector<float> probabilityBins;
    vector<float> weightedProbabilityBins;
    float increment;
    float maxValue;
    /*
     vector< vector<int> > bins;
     vector< vector<float> > probabilityBins;
     vector<float> increment;
     vector<float> maxValue;
     void calculateProbabilityBins();
     */
};

#endif /* Histogram_h */
