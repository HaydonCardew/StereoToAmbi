#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <deque>
#include <vector>

using namespace std;

class WindowedFIFOBuffer
{
private:
	deque<float> inputBuffer;
	deque<float> outputBuffer;
	const int windowSize;
	dsp::WindowingFunction<float> window;
	const float overlap;

public:
	WindowedFIFOBuffer(int windowSize);

	int write(const float *data, int nSamples);
	int read(float *data, int nSamples);

	bool windowedAudioAvailable();
	int outputSamplesAvailable();
	bool getWindowedAudio(vector<float>& buffer);
	bool sendProcessedWindow(vector<float>& buffer);
};
