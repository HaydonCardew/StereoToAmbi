/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "MultiLevelThreshold.h"
#include "Buffers/MultiChannelWindowedFIFOBuffer.h"
#include "Buffers/BFormatBuffer.h"
#include "Deverb.h"

//#define STEREO_DECODER
#define AMBI_ORDER 3

#define WIDTH_NAME "Width"
#define WIDTH_ID "width"
#define OFFSET_NAME "Offset"
#define OFFSET_ID "offset"
#define DEVERB_NAME "Deverb"
#define DEVERB_ID "deverb"
#define DEVERB_THRESHOLD_NAME "DeverbThreshold"
#define DEVERB_THRESHOLD_ID "deverbThreshold"
#define DEVERB_SUSTAIN_NAME "DeverbSustain"
#define DEVERB_SUSTAIN_ID "deverbSustain"

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
    void derverbWrite(float* left, float* right, int nSamples);
    int deverbRead(float* buffer, int nSamples);
    
    AudioProcessorValueTreeState valueTree;
    unsigned getOutputOrder () const { return AMBI_ORDER; }
    unsigned numberOfBFormatChannels () const { return pow(AMBI_ORDER + 1, 2); };
    
private:
    
    enum { LEFT = 0, RIGHT = 1, STEREO = 2 };
    
    static const unsigned fftOrder = 13; // this controls fftSize and windowLength ~13 for MSc setting
    unsigned fftSize;
	unsigned windowLength;
    
    dsp::FFT fft;
    
    MultiChannelWindowedFIFOBuffer stereoAudio;
    
    BFormatBuffer ambiAudio;
    
    MultiLevelThreshold multiLevelThreshold;
    
	atomic<float>* width;
    atomic<float>* offset;
    atomic<float>* extractReverb;
    atomic<float>* deverbThreshold; // should be frequency dependant?
    NormalisableRange<float> deverbThresholdRange;
    atomic<float>* deverbSustain;
    NormalisableRange<float> deverbSustainRange;
    

    Deverb deverb;
    vector<MultiLevelThreshold::ComplexFft> directFreqBuffer, ambientFreqBuffer, ambientTimeBuffer;
    vector<MultiLevelThreshold::ComplexFft> extractedFfts, extractedSources, stereoTimeBuffer, stereoFreqBuffer;
	vector<float> sourceAzimuths;
    
    bool convertParamToBool(float param) { return param > 0.5f ? true : false; };
    
    // test and should be private - initialise after window length though
    MultiChannelWindowedFIFOBuffer extractedAudio;
    MultiChannelWindowedFIFOBuffer deverbAudio;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoToAmbiAudioProcessor)
};
