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
    : AudioProcessorEditor (&p), processor (p)//, orderSelect(orders), channelFormatSelect(channelFormats)
{
	
	/*aziControl.setBounds(30, 30, 300, 300);
	addAndMakeVisible(aziControl);

	orderSelect.setBounds(350, 40, 200, 30);
	addAndMakeVisible(orderSelect);

	channelFormatSelect.setBounds(350, 80, 200, 30);
	addAndMakeVisible(channelFormatSelect);*/
	
    setLookAndFeel(&laf);
    
    background = ImageCache::getFromMemory(Assets::Background_png, Assets::Background_pngSize);
    
    setSize(background.getWidth(), background.getHeight());
    
    azimuthControl.setRange(0.0f, 3.1416f);
    azimuthControl.onValueChange = [this] {angleShown.changeAzimuth(azimuthControl.getValue()); };
    azimuthControl.setSliderStyle(juce::Slider::LinearVertical);
    azimuthControl.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0.0f, 0.0f);

    centreControl.setRange(0.0f, 3.1416f * 2.0f);
    centreControl.onValueChange = [this] {
        listener.rotateBy(centreControl.getValue());
        angleShown.changeCentrePosition(centreControl.getValue());
    };
    
    centreControl.setSliderStyle(juce::Slider::LinearVertical);
    centreControl.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0.0f, 0.0f);

    resized();
    addAndMakeVisible(angleShown);
    addAndMakeVisible(azimuthControl);
    addAndMakeVisible(centreControl);
    getLookAndFeel().setColour(Slider::trackColourId, Colours::darkgrey);
}

StereoToAmbiAudioProcessorEditor::~StereoToAmbiAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void StereoToAmbiAudioProcessorEditor::paint (Graphics& g)
{
    g.drawImageAt(background, 0, 0);
}

/*void StereoToAmbiAudioProcessorEditor::resized() override
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}*/

