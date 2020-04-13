#include "WindowedFIFOBuffer.h"

WindowedFIFOBuffer::WindowedFIFOBuffer(const unsigned windowSize)
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
	if (buffer.size() != windowSize)
    {
		return false;
	}
	unsigned overlapBorder = overlap * windowSize;
	if (outputBuffer.size() < overlapBorder)
    {
        overlapBorder = (unsigned)outputBuffer.size();
    }
	unsigned ptr = (unsigned)outputBuffer.size() - overlapBorder;
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

unsigned WindowedFIFOBuffer::outputSamplesAvailable()
{
    if(outputBuffer.size() < (windowSize*overlap))
    {
        return 0;
    }
	return outputBuffer.size() - (windowSize*overlap);
}

MultiChannelWindowedFIFOBuffer::MultiChannelWindowedFIFOBuffer(unsigned nChannels, unsigned windowSize)
{
    buffers.clear();
    for(int i = 0; i < nChannels; ++i)
    {
        buffers.push_back(make_shared<WindowedFIFOBuffer>(windowSize));
    }
}

shared_ptr<WindowedFIFOBuffer> MultiChannelWindowedFIFOBuffer::getChannel(unsigned channel)
{
    if(channel > buffers.size())
    {
        return nullptr;
    }
    return buffers[channel];
}

bool MultiChannelWindowedFIFOBuffer::windowedAudioAvailable()
{
    for(auto & buffer : buffers)
    {
        if(!buffer->windowedAudioAvailable())
        {
            return false;
        }
    }
    return true;
}

unsigned MultiChannelWindowedFIFOBuffer::outputSamplesAvailable()
{
    unsigned nSamplesAvailable = buffers[0]->outputSamplesAvailable();
    for(vector<shared_ptr<WindowedFIFOBuffer>>::iterator buffer = (buffers.begin()+1); buffer != buffers.end(); ++buffer)
    {
        nSamplesAvailable = min(nSamplesAvailable, (*buffer)->outputSamplesAvailable());
    }
    return nSamplesAvailable;
}
