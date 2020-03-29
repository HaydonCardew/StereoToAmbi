#include "WindowedFIFOBuffer.h"

WindowedFIFOBuffer::WindowedFIFOBuffer(const int windowSize)
: windowSize(windowSize),
window(windowSize, dsp::WindowingFunction<float>::hann),
overlap(0.5f)
{}

int WindowedFIFOBuffer::write(const float *data, int nSamples)
{
	for (int i = 0; i < nSamples; ++i)
    {
		inputBuffer.push_back(data[i]);
	}
	return nSamples;
}

int WindowedFIFOBuffer::read(float *data, int nSamples)
{
	// don't read the last overlap*windowSize, this needs to be added with another window to be valid
	int effectiveOutputBufferSize = outputBuffer.size() - (windowSize*overlap);
	int nRead = (nSamples > effectiveOutputBufferSize) ? effectiveOutputBufferSize : nSamples;
	for (int i = 0; i < nRead; ++i)
    {
		// halve the output as the Hanning windowws cause's +6dB?
		data[i] = outputBuffer[0] * 0.5;
		outputBuffer.pop_front();
	}
	return nRead;
}

bool WindowedFIFOBuffer::getWindowedAudio(vector<float>& buffer)
{
	if (buffer.size() < windowSize || inputBuffer.size() < windowSize)
    {
		return false;
	}
	int border = (1.0f - overlap) * windowSize;
	for (int i = 0; i < border; ++i)
    {
		buffer[i] = inputBuffer[0];
		inputBuffer.pop_front();
	}
	int j = 0;
	for (int i = border; i < windowSize; ++i, ++j)
    {
		buffer[i] = inputBuffer[j];
	}
	window.multiplyWithWindowingTable(&buffer[0], windowSize);
	return true;
}

bool WindowedFIFOBuffer::sendProcessedWindow(vector<float>& buffer)
{
	if (buffer.size() < windowSize) {
		return false;
	}
	int overlapBorder = overlap * windowSize;
	if (outputBuffer.size() < overlapBorder)
    {
        overlapBorder = (int)outputBuffer.size();
    }
	int ptr = (int)outputBuffer.size() - overlapBorder;
	for (int i = 0; i < overlapBorder; ++i)
    {
		outputBuffer[ptr] = outputBuffer[ptr] + buffer[i];
		ptr++;
	}
	for (int i = overlapBorder; i < windowSize; ++i)
    {
		outputBuffer.push_back(buffer[i]);
	}
	return true;
}

bool WindowedFIFOBuffer::windowedAudioAvailable()
{
	return (inputBuffer.size() > windowSize);
}

int WindowedFIFOBuffer::outputSamplesAvailable()
{
	return outputBuffer.size() - (windowSize*overlap);
}
