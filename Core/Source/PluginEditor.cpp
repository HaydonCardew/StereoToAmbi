/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Assets.h"

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
    Image background = ImageCache::getFromMemory(Assets::Background_png,
                                                      Assets::Background_pngSize);
    g.drawImageAt(background.rescaled(396, 134), 194, 181);
}

/*void StereoToAmbiAudioProcessorEditor::resized() override
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}*/

