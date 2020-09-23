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
        Tools::zeroVector(stereoTimeBuffer);
        Tools::zeroVector(stereoFreqBuffer);
        
        for(unsigned channel = 0; channel < STEREO; ++channel)
        {
            stereoAudio.getChannel(channel)->getWindowedAudio(transferBuffer[channel]);
            for (unsigned i = 0; i < windowLength; i++)
            {
                stereoTimeBuffer[channel][i] = transferBuffer[channel][i];
            }
            fft.perform(stereoTimeBuffer[channel].data(), stereoFreqBuffer[channel].data(), false);
        }

        // Perform Stereo to Ambi processing
        float offset = 0.f;
        multiLevelThreshold.stereoFftToAmbiFft(stereoFreqBuffer, extractedFfts, sourceAzimuths, width, offset, sampleRate);
        
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
