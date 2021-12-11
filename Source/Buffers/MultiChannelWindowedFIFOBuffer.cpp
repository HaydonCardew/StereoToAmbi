/*
*  This file is part of StereoToAmbi.
*
*  StereoToAmbi is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  StereoToAmbi is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with StereoToAmbi.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "MultiChannelWindowedFIFOBuffer.h"
#include <assert.h>

MultiChannelWindowedFIFOBuffer::MultiChannelWindowedFIFOBuffer(unsigned nChannels, unsigned windowSize) : windowSize(windowSize)
{
    transferBuffer.resize(nChannels, vector<float>(windowSize, 0));
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
    assert(writeBuffers[0].size() >= windowSize);
    for (unsigned i = 0; i < buffers.size(); ++i)
    {
        assert(buffers[i]->windowedAudioAvailable());
        buffers[i]->getWindowedAudio(writeBuffers[i]);
    }
}

void MultiChannelWindowedFIFOBuffer::getWindowedAudio(vector<vector<complex<float>>>& writeBuffers)
{
    assert(writeBuffers.size() >= buffers.size());
    assert(writeBuffers[0].size() >= windowSize);
    getWindowedAudio(transferBuffer);
    for (unsigned i = 0; i < buffers.size(); ++i)
    {
        for (unsigned j = 0; j < windowSize; ++j)
        {
            writeBuffers[i][j] = transferBuffer[i][j];
        }
    }
}

void MultiChannelWindowedFIFOBuffer::sendProcessedWindows(vector<vector<float>>& readBuffers)
{
    assert(readBuffers.size() >= buffers.size());
    assert(readBuffers[0].size() >= windowSize);
    for (unsigned i = 0; i < buffers.size(); ++i)
    {
        buffers[i]->sendProcessedWindow(readBuffers[i]);
    }
}

void MultiChannelWindowedFIFOBuffer::sendProcessedWindows(vector<vector<complex<float>>>& readBuffers)
{
    assert(readBuffers.size() >= buffers.size());
    assert(readBuffers[0].size() >= windowSize);
    for (unsigned i = 0; i < readBuffers.size(); ++i)
    {
        for (unsigned j = 0; j < windowSize; ++j)
        {
            transferBuffer[i][j] = readBuffers[i][j].real();
        }
    }
    sendProcessedWindows(transferBuffer);
}

unsigned MultiChannelWindowedFIFOBuffer::outputSamplesAvailable()
{
    unsigned nSamplesAvailable = buffers[0]->outputSamplesAvailable();
    for (auto & buffer : buffers)
    {
        nSamplesAvailable = min(nSamplesAvailable, buffer->outputSamplesAvailable());
    }
    return nSamplesAvailable;
}

unsigned MultiChannelWindowedFIFOBuffer::size()
{
    return static_cast<unsigned>(buffers.size());
}

void MultiChannelWindowedFIFOBuffer::clear()
{
    for ( auto & buffer : buffers )
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
