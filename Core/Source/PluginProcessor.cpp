/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StereoToAmbiAudioProcessor::StereoToAmbiAudioProcessor(int nThresholds)
	: fftSize(pow(2,fftOrder)), windowLength(fftSize/2), fft(fftOrder), stereoAudio(2, windowLength), ambiAudio(1, windowLength), multiLevelThreshold(nThresholds, fftSize, 100, 1)
#ifndef JucePlugin_PreferredChannelConfigurations
     , AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo())
                      #endif
                       //.withOutput ("Output", AudioChannelSet::stereo())
                       .withOutput ("Output", AudioChannelSet::ambisonic (1))
                     #endif
                       )
#endif
{
	extractedFfts.resize(multiLevelThreshold.getNumberOfExtractedSources(), MultiLevelThreshold::ComplexFft(fftSize, 0));
	sourceAzimuths.resize(multiLevelThreshold.getNumberOfExtractedSources());
	transferBuffer.resize((nThresholds+1)*2, vector<float>(windowLength));
	extractedSources.resize(multiLevelThreshold.getNumberOfExtractedSources(), vector<dsp::Complex<float>>(fftSize, 0));
	//std::fill(transferBuffer.begin(), transferBuffer.end(), 0);
	leftFreqBuffer.resize(fftSize);
	leftTimeBuffer.resize(fftSize);
	rightFreqBuffer.resize(fftSize);
	rightTimeBuffer.resize(fftSize);
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
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
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
	 && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void StereoToAmbiAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
	auto nSamples = buffer.getNumSamples();

	getSampleRate();

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

    stereoAudio.getChannel(0)->write(buffer.getReadPointer(0), nSamples);
	stereoAudio.getChannel(1)->write(buffer.getReadPointer(1), nSamples);

    while (stereoAudio.windowedAudioAvailable())
    {
        // memcopy and loop
        // Get Left FFT
        stereoAudio.getChannel(0)->getWindowedAudio(transferBuffer[0]);
		for (int i = 0; i < windowLength; i++)
        {
			leftTimeBuffer[i] = transferBuffer[0][i];
		}
		fft.perform(leftTimeBuffer.data(), leftFreqBuffer.data(), false);

        // Get Right FFT
		stereoAudio.getChannel(1)->getWindowedAudio(transferBuffer[0]);
		for (int i = 0; i < windowLength; i++)
        {
			rightTimeBuffer[i] = transferBuffer[0][i];
		}
		fft.perform(rightTimeBuffer.data(), rightFreqBuffer.data(), false);

		// Perform Stereo to Ambi processing
		multiLevelThreshold.stereoFftToAmbiFft(leftFreqBuffer, rightFreqBuffer, extractedFfts, sourceAzimuths, 30);
        
        // ifdef for sending to left/right buffer
        
        // this is just passthrough
        /*
        fft.perform(leftFreqBuffer.data(), leftTimeBuffer.data(), true);
        for (int i = 0; i < windowLength; i++)
        {
            transferBuffer[i] = leftTimeBuffer[i].real();
        }
        stereoAudio.getChannel(0)->sendProcessedWindow(transferBuffer);

        fft.perform(rightFreqBuffer.data(), rightTimeBuffer.data(), true);
        for (int i = 0; i < windowLength; i++)
        {
            transferBuffer[i] = rightTimeBuffer[i].real();
        }
        stereoAudio.getChannel(1)->sendProcessedWindow(transferBuffer);
        */
        // else
        // Convert each object to time and send to Ambi buffer
		for (int i = 0; i < extractedSources.size(); i++)
        {
			fft.perform(extractedFfts[i].data(), extractedSources[i].data(), true);
			for (int j = 0; j < windowLength; j++)
            {
				transferBuffer[i][j] = extractedSources[i][j].real();
			}
            // Load here to an ambiAudio windowed buffer
            //extractedAudio.getChannel(i)->sendProcessedWindow(transferBuffer);
            ambiAudio.addAudioOjectsAsBFormat(transferBuffer, sourceAzimuths);
        }
        // endif
	}
    
    // ifdef for sending left/right chan
	if (ambiAudio.outputSamplesAvailable() >= nSamples)
    {
		//stereoAudio.getChannel(0)->read(buffer.getWritePointer(0), nSamples);
		//stereoAudio.getChannel(1)->read(buffer.getWritePointer(1), nSamples);
        ambiAudio.getChannel(0)->read(buffer.getWritePointer(0), nSamples);
        ambiAudio.getChannel(1)->read(buffer.getWritePointer(1), nSamples);
        ambiAudio.getChannel(2)->read(buffer.getWritePointer(2), nSamples);
        ambiAudio.getChannel(3)->read(buffer.getWritePointer(3), nSamples);
	}
    //else
    // endif
}

void StereoToAmbiAudioProcessor::testProcessBlockWrite(float* left, float* right, int nSamples)
{
	stereoAudio.getChannel(0)->write(left, nSamples);
	stereoAudio.getChannel(1)->write(right, nSamples);
}

int StereoToAmbiAudioProcessor::testProcessBlockRead(float* left, float* right, int nSamples, float* azimuths, float width)
{
    while (stereoAudio.windowedAudioAvailable())
    {
        stereoAudio.getChannel(0)->getWindowedAudio(transferBuffer[0]);
		for (int i = 0; i < windowLength; i++)
        {
			leftTimeBuffer[i] = transferBuffer[0][i];
		}
		fft.perform(leftTimeBuffer.data(), leftFreqBuffer.data(), false);
		stereoAudio.getChannel(1)->getWindowedAudio(transferBuffer[0]);
		for (int i = 0; i < windowLength; i++)
        {
			rightTimeBuffer[i] = transferBuffer[0][i];
		}
		fft.perform(rightTimeBuffer.data(), rightFreqBuffer.data(), false);
		//******

		multiLevelThreshold.stereoFftToAmbiFft(leftFreqBuffer, rightFreqBuffer, extractedFfts, sourceAzimuths, width);
		for (int i = 0; i < extractedSources.size(); i++)
        {
			fft.perform(extractedFfts[i].data(), extractedSources[i].data(), true);
			for (int j = 0; j < windowLength; j++)
            {
				transferBuffer[i][j] = extractedSources[i][j].real();
			}
			//extractedAudio.getChannel(i)->sendProcessedWindow(transferBuffer);
            ambiAudio.addAudioOjectsAsBFormat(transferBuffer, sourceAzimuths);
        }
		//******
        for (int i = 0; i < sourceAzimuths.size(); ++i)
        {
            azimuths[i] = sourceAzimuths[i];
        }
        
		fft.perform(leftFreqBuffer.data(), leftTimeBuffer.data(), true);
		for (int i = 0; i < windowLength; i++)
        {
			transferBuffer[0][i] = leftTimeBuffer[i].real();
		}
		stereoAudio.getChannel(0)->sendProcessedWindow(transferBuffer[0]);
		fft.perform(rightFreqBuffer.data(), rightTimeBuffer.data(), true);
		for (int i = 0; i < windowLength; i++)
        {
			transferBuffer[0][i] = rightTimeBuffer[i].real();
		}
		stereoAudio.getChannel(1)->sendProcessedWindow(transferBuffer[0]);
	}

	if (stereoAudio.outputSamplesAvailable() >= nSamples)
    {
		stereoAudio.getChannel(0)->read(left, nSamples); // if < nSamples left
		stereoAudio.getChannel(1)->read(right, nSamples);
        return nSamples;
	}
    return 0;
}

int StereoToAmbiAudioProcessor::testProcessBlockMultiRead(float* buffer, int nSamples, float* azimuths, float width)
{
    while (stereoAudio.windowedAudioAvailable()) {
        stereoAudio.getChannel(0)->getWindowedAudio(transferBuffer[0]);
        for (int i = 0; i < windowLength; i++) {
            leftTimeBuffer[i] = transferBuffer[0][i];
        }
        fft.perform(leftTimeBuffer.data(), leftFreqBuffer.data(), false);
        stereoAudio.getChannel(1)->getWindowedAudio(transferBuffer[0]);
        for (int i = 0; i < windowLength; i++) {
            rightTimeBuffer[i] = transferBuffer[0][i];
        }
        fft.perform(rightTimeBuffer.data(), rightFreqBuffer.data(), false);
        //******

        multiLevelThreshold.stereoFftToAmbiFft(leftFreqBuffer, rightFreqBuffer, extractedFfts, sourceAzimuths, width);
        for (int i = 0; i < extractedSources.size(); i++) {
            fft.perform(extractedFfts[i].data(), extractedSources[i].data(), true);
            for (int j = 0; j < windowLength; j++) {
                transferBuffer[i][j] = extractedSources[i][j].real();
            }
            //extractedAudio.getChannel(i)->sendProcessedWindow(transferBuffer[0]);
            ambiAudio.addAudioOjectsAsBFormat(transferBuffer, sourceAzimuths);
        }
        //******
        for (int i = 0; i < sourceAzimuths.size(); ++i)
        {
            azimuths[i] = sourceAzimuths[i];
        }
    }

    if (ambiAudio.outputSamplesAvailable() >= nSamples)
    {
        for (int i = 0; i < extractedSources.size(); i++)
        {
            ambiAudio.getChannel(i)->read(buffer, nSamples);
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

extern "C" {
    StereoToAmbiAudioProcessor* STAAP_new(int nThresholds) { return new StereoToAmbiAudioProcessor(nThresholds); }
    void STAAP_write(StereoToAmbiAudioProcessor* ST, float* left, float* right, int nSamples) { return ST->testProcessBlockWrite(left, right, nSamples); }
    int STAAP_read(StereoToAmbiAudioProcessor* ST, float* left, float* right, int nSamples, float* azimuths, float width) { return ST->testProcessBlockRead(left, right, nSamples, azimuths, width); }
    int STAAP_multi_read(StereoToAmbiAudioProcessor* ST, float* buffer, int nSamples, float* azimuths, float width) { return ST->testProcessBlockMultiRead(buffer, nSamples, azimuths, width); }
    void STAAP_getLastHisto(StereoToAmbiAudioProcessor* ST, float* probs, int* bins, int size) { return ST->getLastHisto(probs, bins, size); }
    void STAAP_delete(StereoToAmbiAudioProcessor* ST) { delete ST; }
}
