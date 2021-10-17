/*
  ==============================================================================

    TestPluginProcessor.cpp
    Created: 22 Apr 2020 9:55:45pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "Tools.h"

extern "C" {
    StereoToAmbiAudioProcessor* STAAP_new(int nThresholds) { return new StereoToAmbiAudioProcessor(nThresholds); }
    void STAAP_write(StereoToAmbiAudioProcessor* ST, float* left, float* right, int nSamples) { return ST->testProcessBlockWrite(left, right, nSamples); }
    int STAAP_multi_read(StereoToAmbiAudioProcessor* ST, float* buffer, int nSamples, float* azimuths, float width, unsigned sampleRate) { return ST->testProcessBlockMultiRead(buffer, nSamples, azimuths, width, sampleRate); }
    void STAAP_deverb_write(StereoToAmbiAudioProcessor* ST, float* left, float* right, int nSamples) { return ST->derverbWrite(left, right, nSamples); }
    int STAAP_deverb_read(StereoToAmbiAudioProcessor* ST, float* buffer, int nSamples) { return ST->deverbRead(buffer, nSamples); }
    void STAAP_delete(StereoToAmbiAudioProcessor* ST) { delete ST; }
}

void StereoToAmbiAudioProcessor::testProcessBlockWrite(float* left, float* right, int nSamples)
{
    stereoAudio.write({left, right}, nSamples);
}

int StereoToAmbiAudioProcessor::testProcessBlockMultiRead(float* buffer, int nSamples, float* azimuths, float width, unsigned sampleRate)
{
    while (stereoAudio.windowedAudioAvailable())
    {
        Tools::zeroVector(transferBuffer);
        Tools::zeroVector(stereoTimeBuffer);
        Tools::zeroVector(stereoFreqBuffer);
        
        stereoAudio.getWindowedAudio(transferBuffer);
        for (unsigned channel = 0; channel < STEREO; ++channel)
        {
            for (unsigned i = 0; i < windowLength; i++)
            {
                stereoTimeBuffer[channel][i] = transferBuffer[channel][i];
            }
            fft.perform(stereoTimeBuffer[channel].data(), stereoFreqBuffer[channel].data(), false);
        }
        // Perform Stereo to Ambi processing
        float offset = 0.f;
        multiLevelThreshold.stereoFftToAmbiFft(stereoFreqBuffer, extractedFfts, sourceAzimuths, width, offset, sampleRate);
        
        for (unsigned i = 0; i < extractedSources.size(); i++)
        {
            fft.perform(extractedFfts[i].data(), extractedSources[i].data(), true);
            for (int j = 0; j < windowLength; j++)
            {
                transferBuffer[i][j] = extractedSources[i][j].real();
            }
        }
        extractedAudio.sendProcessedWindows(transferBuffer);
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

void StereoToAmbiAudioProcessor::derverbWrite(float* left, float* right, int nSamples)
{
    stereoAudio.write({left, right}, nSamples);
}

int StereoToAmbiAudioProcessor::deverbRead(float* buffer, int nSamples)
{
    if (transferBuffer.size() != 4)
    {
        transferBuffer.resize(4, vector<float>(windowLength));
    }
    
    while (stereoAudio.windowedAudioAvailable())
    {
        Tools::zeroVector(transferBuffer);
        stereoAudio.getWindowedAudio(transferBuffer);
        for (unsigned channel = 0; channel < STEREO; ++channel)
        {
            for (unsigned i = 0; i < windowLength; i++)
            {
                stereoTimeBuffer[channel][i] = transferBuffer[channel][i];
            }
            fft.perform(stereoTimeBuffer[channel].data(), stereoFreqBuffer[channel].data(), false);
        }
        
        deverb.deverberate(stereoFreqBuffer, directFreqBuffer, ambientFreqBuffer);
        
        for (unsigned i = 0; i < directFreqBuffer.size(); i++)
        {
            fft.perform(directFreqBuffer[i].data(), extractedSources[i].data(), true);
            for (int j = 0; j < windowLength; j++)
            {
                transferBuffer[i][j] = extractedSources[i][j].real();
            }
        }
        for (unsigned i = 0; i < ambientFreqBuffer.size(); i++)
        {
            fft.perform(ambientFreqBuffer[i].data(), extractedSources[i].data(), true);
            for (int j = 0; j < windowLength; j++)
            {
                transferBuffer[i+STEREO][j] = extractedSources[i][j].real();
            }
        }
        deverbAudio.sendProcessedWindows(transferBuffer);
    }
    
    if (deverbAudio.outputSamplesAvailable() >= nSamples)
    {
        for (int i = 0; i < extractedSources.size(); i++)
        {
            deverbAudio.getChannel(i)->read(buffer, nSamples);
            buffer += nSamples;
        }
        return nSamples;
    }
    
    return 0;
}
