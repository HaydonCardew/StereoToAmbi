#include "WindowedFIFOBuffer.h"
#include "Tools.hpp"

WindowedFIFOBuffer::WindowedFIFOBuffer(const unsigned windowSize)
: windowSize(windowSize),
window(windowSize, dsp::WindowingFunction<float>::hann),
overlap(0.5f)
{}

unsigned WindowedFIFOBuffer::write(const float *data, unsigned nSamples)
{
	for (unsigned i = 0; i < nSamples; ++i)
    {
		inputBuffer.push_back(data[i]);
	}
	return nSamples;
}

unsigned WindowedFIFOBuffer::read(float *data, unsigned nSamples)
{
	// don't read the last overlap*windowSize, this needs to be added with another window to be valid
    assert(outputBuffer.size() >= (windowSize*overlap));
	unsigned effectiveOutputBufferSize = outputBuffer.size() - (windowSize*overlap);
	unsigned nRead = (nSamples > effectiveOutputBufferSize) ? effectiveOutputBufferSize : nSamples;
	for (unsigned i = 0; i < nRead; ++i)
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
	unsigned border = (1.0f - overlap) * windowSize;
	for (unsigned i = 0; i < border; ++i)
    {
		buffer[i] = inputBuffer[0];
		inputBuffer.pop_front();
	}
	unsigned j = 0;
	for (unsigned i = border; i < windowSize; ++i, ++j)
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
	for (unsigned i = 0; i < overlapBorder; ++i)
    {
		outputBuffer[ptr] = outputBuffer[ptr] + buffer[i];
		ptr++;
	}
	for (unsigned i = overlapBorder; i < windowSize; ++i)
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
    unsigned minAvail = float(windowSize)*overlap;
    unsigned bufferSize = (unsigned)outputBuffer.size();
    if(bufferSize < minAvail)
    {
        return 0;
    }
	return outputBuffer.size() - (windowSize*overlap);
}

void WindowedFIFOBuffer::clear()
{
    inputBuffer.clear();
    outputBuffer.clear();
}

MultiChannelWindowedFIFOBuffer::MultiChannelWindowedFIFOBuffer(unsigned nChannels, unsigned windowSize)
{
    buffers.clear();
    for(unsigned i = 0; i < nChannels; ++i)
    {
        buffers.push_back(make_shared<WindowedFIFOBuffer>(windowSize));
    }
}

shared_ptr<WindowedFIFOBuffer> MultiChannelWindowedFIFOBuffer::getChannel(unsigned channel)
{
    assert(channel < buffers.size());
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

unsigned MultiChannelWindowedFIFOBuffer::size()
{
    return static_cast<unsigned>(buffers.size());
}

void MultiChannelWindowedFIFOBuffer::clear()
{
    for(auto buffer : buffers)
    {
        buffer->clear();
    }
}

BFormatBuffer::BFormatBuffer(unsigned order, unsigned windowSize)
    : MultiChannelWindowedFIFOBuffer(pow((order+1), 2), windowSize), maxAmbiOrder(order), nAmbiChannels(pow((order+1), 2)), windowSize(windowSize)
{
    bFormatTransferBuffer.resize(nAmbiChannels, vector<float>(windowSize, 0));
    transferBuffer.resize(windowSize, 0);
    furseMalhamCoefs.resize(nAmbiChannels, 0);
}

void BFormatBuffer::addAudioOjectsAsBFormat(const vector<vector<float>>& audioObjects, const vector<float>& azimuths)
{
    assert(audioObjects.size() == azimuths.size());
    assert(audioObjects[0].size() == windowSize);
    Tools::zeroVector(bFormatTransferBuffer);
    for(unsigned i = 0; i < audioObjects.size(); ++i)
    {
        calculateFurseMalhamCoefs(Tools::toRadians(azimuths[i]));
        for(unsigned j = 0; j < windowSize; ++j)
        {
            for(unsigned channel = 0; channel < nAmbiChannels; ++channel)
            {
                bFormatTransferBuffer[channel][j] += audioObjects[i][j] * furseMalhamCoefs[channel];
            }
        }
    }
    for(unsigned i = 0; i < nAmbiChannels; ++i)
    {
        buffers[i]->sendProcessedWindow(bFormatTransferBuffer[i]);
    }
}

void BFormatBuffer::calculateFurseMalhamCoefs(float azimuth)
{
    furseMalhamCoefs[0] = 0.7071;
    furseMalhamCoefs[1] = cos(azimuth);
    furseMalhamCoefs[2] = sin(azimuth);
    furseMalhamCoefs[3] = 0; // elev only
    if(maxAmbiOrder > 1)
    {
        furseMalhamCoefs[4] = -0.5;
        furseMalhamCoefs[5] = 0;
        furseMalhamCoefs[6] = 0;
        furseMalhamCoefs[7] = cos(2*azimuth);
        furseMalhamCoefs[8] = sin(2*azimuth);
    }
    if(maxAmbiOrder > 2)
    {
        furseMalhamCoefs[9] = 0;
        furseMalhamCoefs[10] = -0.7262 * cos(azimuth);
        furseMalhamCoefs[11] = -0.7262 * sin(azimuth);
        furseMalhamCoefs[12] = 0;
        furseMalhamCoefs[13] = 0;
        furseMalhamCoefs[14] = cos(3 * azimuth);
        furseMalhamCoefs[15] = sin(3 * azimuth);
    }
}

void BFormatBuffer::readAsStereo(float* left, float* right, unsigned nSamples)
{
    Tools::zeroVector(transferBuffer);
    buffers[0]->read(&transferBuffer[0], nSamples);
    //memcpy();
    for(unsigned i = 0; i < nSamples; ++i)
    {
        left[i] = transferBuffer[i];
        right[i] = transferBuffer[i];
    }
    buffers[1]->read(&transferBuffer[0], nSamples);
    //memcpy();
    for(unsigned i = 0; i < nSamples; ++i)
    {
        left[i] += transferBuffer[i];
        right[i] -= transferBuffer[i];
    }
    // clear the rest...
    buffers[2]->read(&transferBuffer[0], nSamples);
    buffers[3]->read(&transferBuffer[0], nSamples);
}
