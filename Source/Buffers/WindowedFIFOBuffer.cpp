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

#include "WindowedFIFOBuffer.h"
#include "../Processing/Tools.h"
#define PI 3.14159265359

WindowedFIFOBuffer::WindowedFIFOBuffer(const unsigned windowSize, const float overlap)
: inputBuffer(10000), outputBuffer(10000), overlap(overlap), windowSize(windowSize),
window(windowSize, WindowType::hann), windowType(WindowType::hann)
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
    //float hopSize = 1-overlap;
    switch (windowType)
    {
        case WindowType::hann:
        {
            /*auto hannValue = [](auto point) { return 0.5 - (0.5 * cos(2*PI*(point))); };
            gain = 0;
            float step = hopSize;
            while (step < 1.f)
            {
                gain += hannValue(step);
                step += hopSize;
            }*/
        }
        break;
        default:
            assert(false);
            break;
    }
    return gain * 2; // Windowed in & out so the gain is applied twice
}
