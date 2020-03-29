/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

StereoToAmbiAudioProcessorEditor::StereoToAmbiAudioProcessorEditor (StereoToAmbiAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p), orderSelect(orders), channelFormatSelect(channelFormats)
{
	
	aziControl.setBounds(30, 30, 300, 300);
	addAndMakeVisible(aziControl);

	orderSelect.setBounds(350, 40, 200, 30);
	addAndMakeVisible(orderSelect);

	channelFormatSelect.setBounds(350, 80, 200, 30);
	addAndMakeVisible(channelFormatSelect);

	setSize(600, 300);
}

StereoToAmbiAudioProcessorEditor::~StereoToAmbiAudioProcessorEditor()
{
    
}

//==============================================================================
void StereoToAmbiAudioProcessorEditor::paint (Graphics& g)
{
	g.setColour(Colours::dimgrey);
	g.fillAll();
}

/*void StereoToAmbiAudioProcessorEditor::resized() override
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}*/

