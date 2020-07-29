#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
#include "Histogram.h"

using namespace std;

class MultiLevelThreshold
{
private:
	vector< vector<float> > panMap;
	vector< vector<float> > magnitude;

	vector< vector<float> > Pl;
	vector< vector<float> > Sl;
	vector< vector<float> > Pr;
	vector< vector<float> > Sr;

	vector< vector<float> > leftSourceMagnitudes;
	vector< vector<float> > rightSourceMagnitudes;

	int nHistogramBins;
	int noOfThresholds;
	int fftSize;

	int width;
	const int sourcesPerChannel;
	const int totalNumberOfSources;

	vector<vector<float>> thresholds;
    
    Histogram leftHistogram;
    Histogram rightHistogram;

public:

	typedef vector<dsp::Complex<float>> ComplexFft;

	MultiLevelThreshold(int noOfThresholds, int fftSize, int histogramSize);

	void calcMagnitudeVectors(const vector<ComplexFft>& stereoFft);
	void extractAudioSources(const ComplexFft& leftFft, const ComplexFft& rightFft, vector<ComplexFft>& ambiFfts);
	void generatePanMap();
	void calcHistogram(float minFreq, float maxFreq, int fs);
	void fastMultiLevelthreshold();

    void calculateAzimuths(vector<float>& azimuths, const int width);
    // returns a scaling of -1 <-> 1
    float estimateScaledAngle(const float leftMagnitude, const float rightMagnitude);
    
	void stereoFftToAmbiFft(const vector<ComplexFft>& stereoFfts, vector<ComplexFft>& ambiFfts, vector<float>& azimuths, const unsigned width, const unsigned offset, const unsigned fs);

	unsigned getNumberOfExtractedSources() { return totalNumberOfSources; };
    
    void offsetAngles(vector<float>& azimuths, unsigned offset);
    
    vector<float> getTotalSourceMagnitudes();
};
