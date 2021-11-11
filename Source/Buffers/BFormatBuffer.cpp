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

#include "BFormatBuffer.h"
#include "../Processing/Tools.h"

BFormatBuffer::BFormatBuffer(unsigned order, unsigned windowSize)
    : MultiChannelWindowedFIFOBuffer(pow((order+1), 2), windowSize), maxAmbiOrder(order), nAmbiChannels(pow((order+1), 2)), windowSize(windowSize)
{
    assert(windowSize > 0);
    transferBuffer.resize(nAmbiChannels, vector<float>(windowSize, 0));
    ambiCoefs.resize(nAmbiChannels, 0);
    convertionBuffer.resize(nAmbiChannels, vector<float>(windowSize, 0));
}

/* For some crazy reason azimuths are recorded anti-clockwise */
void BFormatBuffer::addAudioOjectsAsBFormatWithAmbience(const vector<vector<float>>& audioObjects, const vector<float>& azimuths, const vector<vector<float>>& ambience, const float centreAngle, ChannelOrder channelOrder)
{
    assert(transferBuffer[0].size() == windowSize);
    Tools::zeroVector(transferBuffer);
    
    addObjectsToTransferBuffer(audioObjects, azimuths, channelOrder);
    addAmbienceToTransferBuffer(ambience, centreAngle);
    
    sendProcessedWindows(transferBuffer);
    assert(sanityCheck());
}

void BFormatBuffer::addAudioOjectsAsBFormatWithAmbience(const vector<vector<complex<float>>>& audioObjects, const vector<float>& azimuths, const vector<vector<complex<float>>>& ambience, const float centreAngle, ChannelOrder channelOrder)
{
    assert(transferBuffer[0].size() == windowSize);
    Tools::zeroVector(transferBuffer);
    
    addObjectsToTransferBuffer(audioObjects, azimuths, channelOrder);
    addAmbienceToTransferBuffer(ambience, centreAngle);
    
    sendProcessedWindows(transferBuffer);
    assert(sanityCheck());
}

void BFormatBuffer::addAudioOjectsAsBFormat(const vector<vector<float>>& audioObjects, const vector<float>& azimuths, ChannelOrder channelOrder)
{
    assert(transferBuffer[0].size() == windowSize);
    Tools::zeroVector(transferBuffer);
    
    addObjectsToTransferBuffer(audioObjects, azimuths, channelOrder);
    
    sendProcessedWindows(transferBuffer);
    assert(sanityCheck());
}

void BFormatBuffer::addAudioOjectsAsBFormat(const vector<vector<complex<float>>>& audioObjects, const vector<float>& azimuths, ChannelOrder channelOrder)
{
    assert(transferBuffer[0].size() == windowSize);
    Tools::zeroVector(transferBuffer);
    
    addObjectsToTransferBuffer(audioObjects, azimuths, channelOrder);
    
    sendProcessedWindows(transferBuffer);
    assert(sanityCheck());
}

void BFormatBuffer::addObjectsToTransferBuffer(const vector<vector<complex<float>>> &audioObjects, const vector<float> &azimuths, ChannelOrder channelOrder)
{
    assert(audioObjects.size() == azimuths.size());
    assert(audioObjects[0].size() == windowSize);
    for ( unsigned object = 0; object < audioObjects.size(); ++object )
    {
        for (unsigned i = 0; i < windowSize; ++i)
        {
            convertionBuffer[object][i] = audioObjects[object][i].real();
        }
    }
    addObjectsToTransferBuffer(convertionBuffer, azimuths, channelOrder);
}

void BFormatBuffer::addObjectsToTransferBuffer(const vector<vector<float>>& audioObjects, const vector<float>& azimuths, ChannelOrder channelOrder)
{
    assert(audioObjects.size() >= azimuths.size());
    assert(audioObjects[0].size() == windowSize);
    for ( unsigned channel = 0; channel < nAmbiChannels; ++channel )
    {
        for ( unsigned object = 0; object < azimuths.size(); ++object )
        {
            calculateAmbiCoefs(Tools::toRadians(azimuths[object]), channelOrder); // this could be optimised. Redo'ing this over and over for 1 coef each time?
            for ( unsigned j = 0; j < windowSize; ++j)
            {
                transferBuffer[channel][j] += audioObjects[object][j] * ambiCoefs[channel];
            }
        }
    }
}

void BFormatBuffer::addAmbienceToTransferBuffer(const vector<vector<complex<float>>> &ambience, const float centreAngle)
{
    assert(ambience.size() == STEREO);
    assert(ambience[0].size() == windowSize);
    for ( unsigned channel = 0; channel < STEREO; ++channel )
    {
        for ( unsigned i = 0; i < windowSize; ++i )
        {
            convertionBuffer[channel][i] = ambience[channel][i].real();
        }
    }
    addAmbienceToTransferBuffer(convertionBuffer, centreAngle);
}

void BFormatBuffer::addAmbienceToTransferBuffer ( const vector<vector<float>>& ambience, const float centreAngle)
{
    // add ambience as hard L/R in 1st order b-format
    assert(ambience.size() >= STEREO);
    assert(ambience[0].size() >= windowSize);

    calculateAmbiCoefs(Tools::toRadians(centreAngle + 90), AmbiX);
    for ( unsigned j = 0; j < windowSize; ++j)
    {
        transferBuffer[0][j] += ambience[LEFT][j] * ambiCoefs[0];
        transferBuffer[1][j] += ambience[LEFT][j] * ambiCoefs[1];
        transferBuffer[2][j] += ambience[LEFT][j] * ambiCoefs[2];
        transferBuffer[3][j] += ambience[LEFT][j] * ambiCoefs[3];
    }
    
    calculateAmbiCoefs(Tools::toRadians(centreAngle - 90), AmbiX);
    for ( unsigned j = 0; j < windowSize; ++j)
    {
        transferBuffer[0][j] += ambience[RIGHT][j] * ambiCoefs[0];
        transferBuffer[1][j] += ambience[RIGHT][j] * ambiCoefs[1];
        transferBuffer[2][j] += ambience[RIGHT][j] * ambiCoefs[2];
        transferBuffer[3][j] += ambience[RIGHT][j] * ambiCoefs[3];
    }
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
        case AmbiX: // https://www.blueripplesound.com/b-format - double check these
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
        stereoTransferBuffer.resize(nSamples, 0); // a little hacky here..
    }
    Tools::zeroVector(transferBuffer);
    buffers[0]->read(&stereoTransferBuffer[0], nSamples); // W
    for(unsigned i = 0; i < nSamples; ++i)
    {
        left[i] = stereoTransferBuffer[i];
        right[i] = stereoTransferBuffer[i];
    }
    switch (channelOrder)
    {
        case FuMa:
            buffers[1]->read(&stereoTransferBuffer[0], nSamples); // X F<->B
            for(unsigned i = 0; i < nSamples; ++i)
            {
                left[i] += 0.6 * stereoTransferBuffer[i];
                right[i] += 0.6 * stereoTransferBuffer[i];
            }
            
            buffers[2]->read(&stereoTransferBuffer[0], nSamples); // Y L<->R
            for(unsigned i = 0; i < nSamples; ++i)
            {
                left[i] += stereoTransferBuffer[i];
                right[i] -= stereoTransferBuffer[i];
            }
            // clear the rest...
            for(int i = 3; i < buffers.size(); ++i)
            {
                buffers[i]->read(&stereoTransferBuffer[0], nSamples);
            }
            break;
        case AmbiX:
            buffers[3]->read(&stereoTransferBuffer[0], nSamples); // X F<->B
            for(unsigned i = 0; i < nSamples; ++i)
            {
                left[i] += 0.6 * stereoTransferBuffer[i];
                right[i] += 0.6 * stereoTransferBuffer[i];
            }
            
            buffers[1]->read(&stereoTransferBuffer[0], nSamples); // Y L<->R
            for(unsigned i = 0; i < nSamples; ++i)
            {
                left[i] += stereoTransferBuffer[i];
                right[i] -= stereoTransferBuffer[i];
            }
            // clear the rest...
            buffers[2]->read(&stereoTransferBuffer[0], nSamples);
            for(int i = 4; i < buffers.size(); ++i)
            {
                buffers[i]->read(&stereoTransferBuffer[0], nSamples);
            }
            break;
    }
}
