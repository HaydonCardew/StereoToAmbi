#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "FastAudioBuffer.h"
#include <vector>

using namespace std;

class WindowedFIFOBuffer
{
    using WindowType = typename dsp::WindowingFunction<float>::WindowingMethod;
    using WindowFunction = typename dsp::WindowingFunction<float>;
    
public:
    WindowedFIFOBuffer(const unsigned windowSize, const float overlap = 0.75f);

    unsigned write(const float *data, unsigned nSamples, const float gain = 1.f);
	unsigned read(float *data, unsigned nSamples, bool acceptLess = true);

	bool windowedAudioAvailable();
	unsigned outputSamplesAvailable();
	bool getWindowedAudio(vector<float>& buffer);
	bool sendProcessedWindow(vector<float>& buffer);
    
    unsigned inputBufferSize();
    unsigned outputBufferSize();
    void clear();

    // test
    int getWindowedAudio(float *data);
    void sendProcessedWindow(float *data);
    
private:
    const float overlap;
    FastAudioBuffer inputBuffer;
    FastAudioBuffer outputBuffer;
    
    const unsigned windowSize;
    float inverseWindowGainFactor;
    
    float getWindowGain(WindowType windowType, float overlap);
    
    WindowFunction window;
    WindowType windowType;
    
};

class MultiChannelWindowedFIFOBuffer
{
public:
    MultiChannelWindowedFIFOBuffer(unsigned nChannels, unsigned windowSize);
    shared_ptr<WindowedFIFOBuffer> getChannel(unsigned channel);
    bool windowedAudioAvailable();
    void write(vector<const float*> readBuffers, unsigned nSamples, float gain = 1.0);
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
