#include "WindowedFIFOBuffer.h"
#include "Tools.h"

#define PI 3.14159265359

WindowedFIFOBuffer::WindowedFIFOBuffer(const unsigned windowSize, const float overlap)
: overlap(overlap), windowSize(windowSize),
window(windowSize, WindowType::hann), windowType(WindowType::hann),
inputBuffer(10000), outputBuffer(10000)
{
    assert(windowSize > 0);
    inverseWindowGainFactor = 1/getWindowGain(windowType, overlap);
}

unsigned WindowedFIFOBuffer::write(const float *data, unsigned nSamples, const float gain)
{
 	for (unsigned i = 0; i < nSamples; ++i)
    {
		inputBuffer.push_back(data[i] * gain);
	}
	return nSamples;
}

unsigned WindowedFIFOBuffer::read(float *data, unsigned nSamples, bool acceptLess)
{
	// don't read the last overlap*windowSize, this needs to be added with another window to be valid
    assert(outputBuffer.size() >= (windowSize*overlap));
	unsigned effectiveOutputBufferSize = outputBuffer.size() - (windowSize*overlap);
    if ( effectiveOutputBufferSize < nSamples)
    {
        nSamples = acceptLess ? effectiveOutputBufferSize : 0;
    }
	for (unsigned i = 0; i < nSamples; ++i)
    {
        data[i] = outputBuffer[0] * inverseWindowGainFactor;
		outputBuffer.pop_front();
	}
	return nSamples;
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
    assert(buffer.size() == windowSize);
    
    window.multiplyWithWindowingTable(&buffer[0], windowSize);
    
	unsigned overlapBorder = overlap * windowSize;
	if (outputBuffer.size() < overlapBorder)
    {
        overlapBorder = outputBuffer.size();
    }
	unsigned ptr = outputBuffer.size() - overlapBorder;
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

unsigned WindowedFIFOBuffer::inputBufferSize()
{
    return (unsigned)inputBuffer.size();
}

unsigned WindowedFIFOBuffer::outputBufferSize()
{
    return (unsigned)outputBuffer.size();
}

void WindowedFIFOBuffer::clear()
{
    inputBuffer.clear();
    outputBuffer.clear();
}

float WindowedFIFOBuffer::getWindowGain(WindowType windowType, float overlap)
{
    float gain = 1.f;
    float hopSize = 1-overlap;
    switch (windowType)
    {
        case WindowType::hann:
            auto hannValue = [](auto point) { return 0.5 - (0.5 * cos(2*PI*(point))); };
            gain = 0;
            float step = hopSize;
            while (step < 1.f)
            {
                gain += hannValue(step);
                step += hopSize;
            }
            break;
    }
    return gain * 2; // Windowed in & out so the gain is applied twice
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

void MultiChannelWindowedFIFOBuffer::write(vector<const float*> readBuffers, unsigned nSamples, float gain)
{
    assert(readBuffers.size() >= buffers.size());
    for (unsigned i = 0; i < buffers.size(); ++i)
    {
        buffers[i]->write(readBuffers[i], nSamples, gain);
    }
}

void MultiChannelWindowedFIFOBuffer::read(vector<float*> writeBuffers, unsigned nSamples)
{
    assert(writeBuffers.size() >= buffers.size());
    for (unsigned i = 0; i < buffers.size(); ++i)
    {
        assert(buffers[i]->outputSamplesAvailable() >= nSamples);
        buffers[i]->read(writeBuffers[i], nSamples);
    }
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

void MultiChannelWindowedFIFOBuffer::getWindowedAudio(vector<vector<float>>& writeBuffers)
{
    assert(writeBuffers.size() >= buffers.size());
    for (unsigned i = 0; i < buffers.size(); ++i)
    {
        assert(buffers[i]->windowedAudioAvailable());
        buffers[i]->getWindowedAudio(writeBuffers[i]);
    }
}

unsigned MultiChannelWindowedFIFOBuffer::outputSamplesAvailable()
{
    unsigned nSamplesAvailable = buffers[0]->outputSamplesAvailable();
    for ( vector<shared_ptr<WindowedFIFOBuffer>>::iterator buffer = (buffers.begin()+1); buffer != buffers.end(); ++buffer)
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
    for ( auto buffer : buffers )
    {
        buffer->clear();
    }
}

// should only be inside an assert
bool MultiChannelWindowedFIFOBuffer::sanityCheck()
{
    unsigned nBuffers = (unsigned)buffers.size();
    if ( nBuffers == 0 )
    {
        return false;
    }
    unsigned inputBufferSize = buffers[0]->inputBufferSize();
    unsigned outputBufferSize = buffers[0]->outputBufferSize();
    for ( unsigned i = 1; i < nBuffers; ++i )
    {
        if ( buffers[i]->inputBufferSize() != inputBufferSize ||
           buffers[i]->outputBufferSize() != outputBufferSize )
        {
            return false;
        }
    }
    return true;
}

BFormatBuffer::BFormatBuffer(unsigned order, unsigned windowSize)
    : MultiChannelWindowedFIFOBuffer(pow((order+1), 2), windowSize), maxAmbiOrder(order), nAmbiChannels(pow((order+1), 2)), windowSize(windowSize)
{
    assert(windowSize > 0);
    transferBuffer.resize(windowSize, 0);
    ambiCoefs.resize(nAmbiChannels, 0);
}

/* For some crazy reason azimuths are recorded anti-clockwise */
void BFormatBuffer::addAudioOjectsAsBFormat(const vector<vector<float>>& audioObjects, const vector<float>& azimuths, ChannelOrder channelOrder)
{
    assert(transferBuffer.size() == windowSize);
    assert(audioObjects.size() == azimuths.size());
    assert(audioObjects[0].size() == windowSize);
    for ( unsigned channel = 0; channel < nAmbiChannels; ++channel )
    {
        Tools::zeroVector(transferBuffer);
        for ( unsigned object = 0; object < audioObjects.size(); ++object )
        {
            calculateAmbiCoefs(Tools::toRadians(azimuths[object]), channelOrder); // this could be optimised. Redo'ing this over and over for 1 coef each time?
            for ( unsigned j = 0; j < windowSize; ++j)
            {
                transferBuffer[j] += audioObjects[object][j] * ambiCoefs[channel];
            }
        }
        buffers[channel]->sendProcessedWindow(transferBuffer);
    }
    assert(sanityCheck());
}

/* For some crazy reason azimuths are recorded anti-clockwise */
void BFormatBuffer::calculateAmbiCoefs(float azimuth, ChannelOrder channelOrder)
{
    switch (channelOrder)
    {
        case FuMa:
            ambiCoefs[0] = 0.7071;
            ambiCoefs[1] = cos(azimuth);
            ambiCoefs[2] = sin(azimuth);
            ambiCoefs[3] = 0; // elev only
            if(maxAmbiOrder > 1)
            {
                ambiCoefs[4] = -0.5;
                ambiCoefs[5] = 0;
                ambiCoefs[6] = 0;
                ambiCoefs[7] = cos(2*azimuth);
                ambiCoefs[8] = sin(2*azimuth);
            }
            if(maxAmbiOrder > 2)
            {
                ambiCoefs[9] = 0;
                ambiCoefs[10] = -0.7262 * cos(azimuth);
                ambiCoefs[11] = -0.7262 * sin(azimuth);
                ambiCoefs[12] = 0;
                ambiCoefs[13] = 0;
                ambiCoefs[14] = cos(3 * azimuth);
                ambiCoefs[15] = sin(3 * azimuth);
            }
            break;
        case SN3D: // https://www.blueripplesound.com/b-format - double check these
            ambiCoefs[0] = 1;
            ambiCoefs[1] = sin(azimuth);
            ambiCoefs[2] = 0; // elev only
            ambiCoefs[3] = cos(azimuth);
            if(maxAmbiOrder > 1)
            {
                ambiCoefs[8] = -0.5;
                ambiCoefs[6] = 0;
                ambiCoefs[4] = 0;
                ambiCoefs[5] = cos(2*azimuth);
                ambiCoefs[7] = sin(2*azimuth);
            }
            if(maxAmbiOrder > 2)
            {
                ambiCoefs[15] = 0;
                ambiCoefs[13] = -0.7262 * cos(azimuth);
                ambiCoefs[11] = -0.7262 * sin(azimuth);
                ambiCoefs[9] = 0;
                ambiCoefs[10] = 0;
                ambiCoefs[12] = cos(3 * azimuth);
                ambiCoefs[14] = sin(3 * azimuth);
            }
            break;
    }
}

void BFormatBuffer::readAsStereo(float* left, float* right, unsigned nSamples, ChannelOrder channelOrder)
{
    assert(sanityCheck());
    if(transferBuffer.size() < nSamples)
    {
        transferBuffer.resize(nSamples, 0); // a little hacky here..
    }
    Tools::zeroVector(transferBuffer);
    buffers[0]->read(&transferBuffer[0], nSamples); // W
    for(unsigned i = 0; i < nSamples; ++i)
    {
        left[i] = transferBuffer[i];
        right[i] = transferBuffer[i];
    }
    switch (channelOrder)
    {
        case FuMa:
            buffers[1]->read(&transferBuffer[0], nSamples); // X F<->B
            for(unsigned i = 0; i < nSamples; ++i)
            {
                left[i] += 0.6 * transferBuffer[i];
                right[i] += 0.6 * transferBuffer[i];
            }
            
            buffers[2]->read(&transferBuffer[0], nSamples); // Y L<->R
            for(unsigned i = 0; i < nSamples; ++i)
            {
                left[i] += transferBuffer[i];
                right[i] -= transferBuffer[i];
            }
            // clear the rest...
            for(int i = 3; i < buffers.size(); ++i)
            {
                buffers[i]->read(&transferBuffer[0], nSamples);
            }
            break;
        case SN3D:
            buffers[3]->read(&transferBuffer[0], nSamples); // X F<->B
            for(unsigned i = 0; i < nSamples; ++i)
            {
                left[i] += 0.6 * transferBuffer[i];
                right[i] += 0.6 * transferBuffer[i];
            }
            
            buffers[1]->read(&transferBuffer[0], nSamples); // Y L<->R
            for(unsigned i = 0; i < nSamples; ++i)
            {
                left[i] += transferBuffer[i];
                right[i] -= transferBuffer[i];
            }
            // clear the rest...
            buffers[2]->read(&transferBuffer[0], nSamples);
            for(int i = 4; i < buffers.size(); ++i)
            {
                buffers[i]->read(&transferBuffer[0], nSamples);
            }
            break;
    }
}
