/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Tools.h"
#include <algorithm>

//==============================================================================
StereoToAmbiAudioProcessor::StereoToAmbiAudioProcessor(int nThresholds)
	: fftSize(pow(2,fftOrder)), windowLength(fftSize), fft(fftOrder), stereoAudio(STEREO, windowLength), ambiAudio(MAX_AMBI_ORDER, windowLength), multiLevelThreshold(nThresholds, fftSize, 100), extractedAudio((nThresholds+1)*STEREO, windowLength)
#ifndef JucePlugin_PreferredChannelConfigurations
     , AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo())
                      #endif
                        #ifdef STEREO_DECODER
                       .withOutput ("Output", AudioChannelSet::stereo())
                        #else
                       .withOutput ("Output", AudioChannelSet::ambisonic (MAX_AMBI_ORDER))
                        #endif
                     #endif
                       ),
valueTree(*this, nullptr, "ValueTree",
{
    std::make_unique<AudioParameterFloat>(WIDTH_ID, WIDTH_NAME, 0.0f, 360.f, 90.0f),
    std::make_unique<AudioParameterFloat>(OFFSET_ID, OFFSET_NAME, 0.0f, 360.f, 0.0f)
})
#endif
{
	extractedFfts.resize(multiLevelThreshold.getNumberOfExtractedSources(), MultiLevelThreshold::ComplexFft(fftSize, 0));
	sourceAzimuths.resize(multiLevelThreshold.getNumberOfExtractedSources());
	transferBuffer.resize(multiLevelThreshold.getNumberOfExtractedSources(), vector<float>(windowLength));
	extractedSources.resize(multiLevelThreshold.getNumberOfExtractedSources(), MultiLevelThreshold::ComplexFft(fftSize, 0));
    stereoFreqBuffer.resize(STEREO, MultiLevelThreshold::ComplexFft(fftSize, 0));
    stereoTimeBuffer.resize(STEREO, MultiLevelThreshold::ComplexFft(fftSize, 0));
    
    width = valueTree.getRawParameterValue(WIDTH_ID);
    offset = valueTree.getRawParameterValue(OFFSET_ID);
}

StereoToAmbiAudioProcessor::~StereoToAmbiAudioProcessor()
{
}

//==============================================================================
const String StereoToAmbiAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StereoToAmbiAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool StereoToAmbiAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool StereoToAmbiAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double StereoToAmbiAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int StereoToAmbiAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int StereoToAmbiAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StereoToAmbiAudioProcessor::setCurrentProgram (int index)
{
}

const String StereoToAmbiAudioProcessor::getProgramName (int index)
{
    return {};
}

void StereoToAmbiAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void StereoToAmbiAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    stereoAudio.clear();
    ambiAudio.clear();
}

void StereoToAmbiAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StereoToAmbiAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainInputChannelSet() != AudioChannelSet::stereo()
        #ifdef STEREO_DECODER
    && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        #else
    && layouts.getMainOutputChannelSet() != AudioChannelSet::ambisonic (MAX_AMBI_ORDER))
        #endif
        return false;

    // This checks if the input layout matches the output layout
/*
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif
*/
    return true;
  #endif
}
#endif

void StereoToAmbiAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    unsigned totalNumInputChannels  = getTotalNumInputChannels();
    unsigned totalNumOutputChannels = getTotalNumOutputChannels();
	auto nSamples = buffer.getNumSamples();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

	//buffer.clear(1, 0, nSamples);

    stereoAudio.getChannel(0)->write(buffer.getReadPointer(0), nSamples, 0.5);
    stereoAudio.getChannel(1)->write(buffer.getReadPointer(1), nSamples, 0.5); // Stereo source can collapse into one source so half the overall gain on input

    while (stereoAudio.windowedAudioAvailable())
    {
        Tools::zeroVector(transferBuffer);
        Tools::zeroVector(stereoTimeBuffer);
        Tools::zeroVector(stereoFreqBuffer);
        
        for (unsigned channel = 0; channel < STEREO; ++channel)
        {
            stereoAudio.getChannel(channel)->getWindowedAudio(transferBuffer[channel]);
            for (unsigned i = 0; i < windowLength; i++)
            {
                stereoTimeBuffer[channel][i] = transferBuffer[channel][i];
            }
            fft.perform(stereoTimeBuffer[channel].data(), stereoFreqBuffer[channel].data(), false);
        }

		// Perform Stereo to Ambi processing
		multiLevelThreshold.stereoFftToAmbiFft(stereoFreqBuffer, extractedFfts, sourceAzimuths, *width, *offset, getSampleRate());
        
		for (unsigned i = 0; i < extractedSources.size(); i++)
        {
			fft.perform(extractedFfts[i].data(), extractedSources[i].data(), true);
			for (int j = 0; j < windowLength; j++)
            {
				transferBuffer[i][j] = extractedSources[i][j].real();
			}
        }
        ambiAudio.addAudioOjectsAsBFormat(transferBuffer, sourceAzimuths); // window audio again as a test
	}
    
	if (ambiAudio.outputSamplesAvailable() >= nSamples)
    {
    #ifdef STEREO_DECODER
        ambiAudio.readAsStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), nSamples);
    #else
        unsigned nChannelsToWrite = min(totalNumOutputChannels, ambiAudio.size());
        for (unsigned i = 0; i < nChannelsToWrite; ++i)
        {
            ambiAudio.getChannel(i)->read(buffer.getWritePointer(i), nSamples);
        }
    #endif
	}
}

//==============================================================================
bool StereoToAmbiAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* StereoToAmbiAudioProcessor::createEditor()
{
    return new StereoToAmbiAudioProcessorEditor (*this);
}

//==============================================================================
void StereoToAmbiAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void StereoToAmbiAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StereoToAmbiAudioProcessor(2);
}
