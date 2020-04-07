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
	int ambiOrder;
	int noOfThresholds;
	int fftSize;

	vector< vector<float> > azimuth;
	int width;
	const int sourcesPerChannel;
	const int totalNumberOfSources;

	vector<vector<float>> thresholds;
    
    //void zeroVector(vector<vector<float>> &input);
    
    Histogram leftHistogram;
    Histogram rightHistogram;

public:

	typedef vector<dsp::Complex<float>> ComplexFft;

	MultiLevelThreshold(int noOfThresholds, int fftSize, int histogramSize, int ambiOrder);

	void calcMagnitudeVectors(const ComplexFft& leftFft, const ComplexFft& rightFft);
	void extractAudioSources(const ComplexFft& leftFft, const ComplexFft& rightFft, vector<ComplexFft>& ambiFfts, vector<float>& azimuths, const int width);
	void generatePanMap();
	void calcHistogram(float minFreq, float maxFreq, int fs);
	void fastMultiLevelthreshold();

    // returns a scaling of -1 <-> 1
    float estimateScaledAngle(float leftMagnitude, float rightMagnitude);
    
	void stereoFftToAmbiFft(const ComplexFft& leftFft, const ComplexFft& rightFft, vector<ComplexFft>& ambiFfts, vector<float>& azimuths, const int width, const int offset = 0);

	int getNumberOfExtractedSources() { return totalNumberOfSources; };
    
    void offsetAngles(vector<float>& azimuths, const int offset);
    
    // test
    void getLastHisto(vector<int>& leftBin, vector<float>& leftProb);
    void testMultiLevelThreshold(float* data, int nDataPoints, float* thresholds, int nThresholds, int nBins);
};
