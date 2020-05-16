/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MultiLevelThreshold.h"
#include "WindowedFIFOBuffer.h"

//==============================================================================
/**
*/
class StereoToAmbiAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    StereoToAmbiAudioProcessor(int nThresholds);
    ~StereoToAmbiAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Test
	void testProcessBlockWrite(float* left, float* right, int nSamples);
    int testProcessBlockMultiRead(float* buffer, int nSamples, float* azimuths, float width, unsigned sampleRate);
    void getLastHisto(float* probs, int* bins, int nSize);
    
private:
    unsigned fftOrder = 13; // this controls fftSize and windowLength ~13 for MSc setting
    unsigned fftSize;
	unsigned windowLength;

	dsp::FFT fft;
    MultiChannelWindowedFIFOBuffer stereoAudio;
    BFormatBuffer ambiAudio;

	MultiLevelThreshold multiLevelThreshold;
    //MultiLevelThreshold::ComplexFft leftFreqBuffer, leftTimeBuffer, rightFreqBuffer, rightTimeBuffer;
    vector<MultiLevelThreshold::ComplexFft> extractedFfts, extractedSources, stereoTimeBuffer, stereoFreqBuffer;
	vector<float> sourceAzimuths;
    vector<vector<float>> transferBuffer;
    
    // test and should be private - initialise after window length though
    MultiChannelWindowedFIFOBuffer extractedAudio;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoToAmbiAudioProcessor)
};
