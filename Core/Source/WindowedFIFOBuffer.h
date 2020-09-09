#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <deque>
#include <vector>

using namespace std;

class WindowedFIFOBuffer
{
public:
	WindowedFIFOBuffer(const unsigned windowSize);

	unsigned write(const float *data, unsigned nSamples);
	unsigned read(float *data, unsigned nSamples);

	bool windowedAudioAvailable();
	unsigned outputSamplesAvailable();
	bool getWindowedAudio(vector<float>& buffer);
	bool sendProcessedWindow(const vector<float>& buffer);
    
    unsigned inputBufferSize();
    unsigned outputBufferSize();
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
    
    //for debug
    bool sanityCheck();
};

class BFormatBuffer : public MultiChannelWindowedFIFOBuffer
{
public:
    BFormatBuffer(unsigned maxOrder, unsigned windowSize);
    
    enum ChannelOrder { SN3D, FuMa };
    
    void addAudioOjectsAsBFormat(const vector<vector<float>>& audioObjects, const vector<float>& azimuths, ChannelOrder channelOrder=SN3D);
    
    // simple stereo decoder for sanity checking
    void readAsStereo(float* left, float* right, unsigned nSamples, ChannelOrder channelOrder=SN3D);
    
private:
    unsigned maxAmbiOrder;
    unsigned nAmbiChannels;
    const unsigned windowSize;
    vector<float> transferBuffer;
    void calculateAmbiCoefs(float sourceAzimuth, ChannelOrder channelOrder);
    vector<float> ambiCoefs;
};
