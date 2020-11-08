#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "FastAudioBuffer.h"
#include <vector>
#include <complex>

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