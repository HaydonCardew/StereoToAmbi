/*
  ==============================================================================

    TestPluginProcessor.cpp
    Created: 22 Apr 2020 9:55:45pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "Tools.hpp"

extern "C" {
    StereoToAmbiAudioProcessor* STAAP_new(int nThresholds) { return new StereoToAmbiAudioProcessor(nThresholds); }
    void STAAP_write(StereoToAmbiAudioProcessor* ST, float* left, float* right, int nSamples) { return ST->testProcessBlockWrite(left, right, nSamples); }
    int STAAP_multi_read(StereoToAmbiAudioProcessor* ST, float* buffer, int nSamples, float* azimuths, float width, unsigned sampleRate) { return ST->testProcessBlockMultiRead(buffer, nSamples, azimuths, width, sampleRate); }
    void STAAP_getLastHisto(StereoToAmbiAudioProcessor* ST, float* probs, int* bins, int size) { return ST->getLastHisto(probs, bins, size); }
    void STAAP_delete(StereoToAmbiAudioProcessor* ST) { delete ST; }
}

void StereoToAmbiAudioProcessor::testProcessBlockWrite(float* left, float* right, int nSamples)
{
	stereoAudio.getChannel(0)->write(left, nSamples);
	stereoAudio.getChannel(1)->write(right, nSamples);
}

int StereoToAmbiAudioProcessor::testProcessBlockMultiRead(float* buffer, int nSamples, float* azimuths, float width, unsigned sampleRate)
{
    while (stereoAudio.windowedAudioAvailable())
    {
        Tools::zeroVector(transferBuffer);
        Tools::zeroVector(leftTimeBuffer);
        Tools::zeroVector(leftFreqBuffer);
        Tools::zeroVector(rightTimeBuffer);
        Tools::zeroVector(rightFreqBuffer);
        
        stereoAudio.getChannel(0)->getWindowedAudio(transferBuffer[0]);
        stereoAudio.getChannel(1)->getWindowedAudio(transferBuffer[1]);
        for (int i = 0; i < windowLength; i++)
        {
            leftTimeBuffer[i] = transferBuffer[0][i];
            rightTimeBuffer[i] = transferBuffer[1][i];
        }
        fft.perform(leftTimeBuffer.data(), leftFreqBuffer.data(), false);
        fft.perform(rightTimeBuffer.data(), rightFreqBuffer.data(), false);

        // Perform Stereo to Ambi processing
        unsigned offset = 0;
        multiLevelThreshold.stereoFftToAmbiFft(leftFreqBuffer, rightFreqBuffer, extractedFfts, sourceAzimuths, width, offset, sampleRate);
        
        for (int i = 0; i < extractedSources.size(); i++)
        {
            fft.perform(extractedFfts[i].data(), extractedSources[i].data(), true);
            azimuths[i] = sourceAzimuths[i];
            for (int j = 0; j < windowLength; j++)
            {
                transferBuffer[i][j] = extractedSources[i][j].real();
            }
            extractedAudio.getChannel(i)->sendProcessedWindow(transferBuffer[i]);
        }
    }
    if (extractedAudio.outputSamplesAvailable() >= nSamples)
    {
        for (int i = 0; i < extractedSources.size(); i++)
        {
            extractedAudio.getChannel(i)->read(buffer, nSamples);
            buffer += nSamples;
        }
        return nSamples;
    }
    
    return 0;
}

void StereoToAmbiAudioProcessor::getLastHisto(float* probs, int* bins, int nSize)
{
    
    vector<float> thisProbs;
    vector<int> thisBins;
    multiLevelThreshold.getLastHisto(thisBins, thisProbs);
    if (nSize < thisBins.size())
    {
        cout << "Fucked histo bins pointer sizes" << endl;
        return;
    }
    for(int i = 0; i < thisBins.size(); ++i)
    {
        probs[i] = thisProbs[i];
        bins[i] = thisBins[i];
    }
    return;
}
