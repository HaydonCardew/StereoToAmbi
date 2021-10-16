/*
  ==============================================================================

    BFormatBuffer.h
    Created: 8 Nov 2020 4:53:39pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include "MultiChannelWindowedFIFOBuffer.h"

class BFormatBuffer : public MultiChannelWindowedFIFOBuffer
{
public:
    BFormatBuffer(unsigned maxOrder, unsigned windowSize);
    
    enum ChannelOrder { AmbiX, FuMa };
    enum { LEFT = 0, RIGHT = 1, STEREO = 2 };
    
    void addAudioOjectsAsBFormat(const vector<vector<float>>& audioObjects, const vector<float>& azimuths, ChannelOrder channelOrder=AmbiX);
    void addAudioOjectsAsBFormat(const vector<vector<complex<float>>>& audioObjects, const vector<float>& azimuths, ChannelOrder channelOrder=AmbiX);
                                 
    void addAudioOjectsAsBFormatWithAmbience(const vector<vector<float>>& audioObjects, const vector<float>& azimuths, const vector<vector<float>>& ambience, const float centreAngle, ChannelOrder channelOrder=AmbiX);
    void addAudioOjectsAsBFormatWithAmbience(const vector<vector<complex<float>>>& audioObjects, const vector<float>& azimuths, const vector<vector<complex<float>>>& ambience, const float centreAngle, ChannelOrder channelOrder=AmbiX);
    
    // simple stereo decoder for sanity checking
    void readAsStereo(float* left, float* right, unsigned nSamples, ChannelOrder channelOrder=AmbiX);
    
private:
    unsigned maxAmbiOrder;
    unsigned nAmbiChannels;
    const unsigned windowSize;
    vector<float> stereoTransferBuffer;
    vector<vector<float>> convertionBuffer;
    void addObjectsToTransferBuffer (const vector<vector<float>>& audioObjects, const vector<float>& azimuths, ChannelOrder channelOrder);
    void addObjectsToTransferBuffer(const vector<vector<complex<float>>> &audioObjects, const vector<float> &azimuths, ChannelOrder channelOrder);
    void addAmbienceToTransferBuffer ( const vector<vector<float>>& ambience, const float centreAngle);
    void addAmbienceToTransferBuffer(const vector<vector<complex<float>>> &ambience, const float centreAngle);
    void calculateAmbiCoefs (float sourceAzimuth, ChannelOrder channelOrder);
    vector<float> ambiCoefs;
};
