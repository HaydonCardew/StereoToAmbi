#include "WindowedFIFOBuffer.h"
#include "Tools.hpp"

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

unsigned MultiChannelWindowedFIFOBuffer::size()
{
    return static_cast<unsigned>(buffers.size());
}

BFormatBuffer::BFormatBuffer(unsigned order, unsigned windowSize)
    : MultiChannelWindowedFIFOBuffer(pow((order+1), 2), windowSize), maxAmbiOrder(order), nAmbiChannels(pow((order+1), 2)), windowSize(windowSize)
{
    bFormatTransferBuffer.resize(nAmbiChannels, vector<float>(windowSize, 0));
}

void BFormatBuffer::addAudioOjectsAsBFormat(const vector<vector<float>>& audioObjects, const vector<float>& azimuths)
{
    //assert audio objects size == azimuth size
    //assert audio objects[0].size == windowSize
    Tools::zeroVector(bFormatTransferBuffer);

    float toRads = 3.14159265/180.f;
    
    //just 1st order atm
    for(unsigned i = 0; i < audioObjects.size(); ++i)
    {
        float firstChannelMultiplier = 0.7071;
        float secondChannelMultiplier = cos(double(azimuths[i]*toRads));
        float thirdChannelMultiplier = sin(double(azimuths[i]*toRads));
        
        for(unsigned j = 0; j < windowSize; ++j)
        {
            bFormatTransferBuffer[0][j] += audioObjects[i][j] * firstChannelMultiplier;
            bFormatTransferBuffer[1][j] += audioObjects[i][j] * secondChannelMultiplier;
            bFormatTransferBuffer[2][j] += audioObjects[i][j] * thirdChannelMultiplier;
        }
    }
    for(unsigned i = 0; i < nAmbiChannels; ++i)
    {
        buffers[i]->sendProcessedWindow(bFormatTransferBuffer[i]);
    }
}

/*
output(:,1) = input .* 0.7071; %X
output(:,2) = input .* cosd(azimuth);
%Y
output(:,3) = input .* sind(azimuth);
%Z
% channel for elevation only, so stay as zero
% 2nd order
if order > 1
133
%R
output(:,5) = input .* -0.5;
%S
% channel zero'd by sin(2E)
%T
% channel zero'd by sin(2E)
%U
output(:,8) = input .* cosd(2 * azimuth); %V
output(:,9) = input .* sind(2 * azimuth);
end
% 3rd order
if order > 2 %K
% channel for elevation only, so stay as zero %L
output(:,11) = input .* -0.7262 * cosd(azimuth); %M
output(:,12) = input .* -0.7262 * sind(azimuth);
%N
% channel zero'd by sin(2E)
%O
% channel zero'd by sin(2E)
%P
output(:,15) = input .* cosd(3 * azimuth); %Q
output(:,16) = input .* sind(3 * azimuth);
end
*/
