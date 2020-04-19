#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <deque>
#include <vector>

using namespace std;

class WindowedFIFOBuffer
{
public:
	WindowedFIFOBuffer(const unsigned windowSize);

	int write(const float *data, int nSamples);
	int read(float *data, int nSamples);

	bool windowedAudioAvailable();
	unsigned outputSamplesAvailable();
	bool getWindowedAudio(vector<float>& buffer);
	bool sendProcessedWindow(vector<float>& buffer);
    void clear();

private:
    deque<float> inputBuffer;
    deque<float> outputBuffer;
    const unsigned windowSize;
    dsp::WindowingFunction<float> window;
    const float overlap;
};

class MultiChannelWindowedFIFOBuffer
{
public:
    MultiChannelWindowedFIFOBuffer(unsigned nChannels, unsigned windowSize);
    shared_ptr<WindowedFIFOBuffer> getChannel(unsigned channel);
    bool windowedAudioAvailable();
    unsigned outputSamplesAvailable();
    unsigned size();
    void clear();
    
protected:
    vector<shared_ptr<WindowedFIFOBuffer>> buffers;
};

class BFormatBuffer : public MultiChannelWindowedFIFOBuffer
{
public:
    BFormatBuffer(unsigned maxOrder, unsigned windowSize);
    void addAudioOjectsAsBFormat(const vector<vector<float>>& audioObjects, const vector<float>& azimuths);
    
    // simple stereo decoder for sanity checking
    void readAsStereo(float* left, float* right, unsigned nSamples);
    
private:
    unsigned maxAmbiOrder;
    unsigned nAmbiChannels;
    const unsigned windowSize;
    vector<vector<float>> bFormatTransferBuffer;
    vector<float> transferBuffer;
    void calculateFurseMalhamCoefs(float sourceAzimuth);
    vector<float> furseMalhamCoefs;
};
