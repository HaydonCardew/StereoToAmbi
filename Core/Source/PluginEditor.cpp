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
}

void StereoToAmbiAudioProcessorEditor::resized()
{
}

StereoToAmbiAudioProcessorEditor::~StereoToAmbiAudioProcessorEditor()
{
}

//==============================================================================
void StereoToAmbiAudioProcessorEditor::paint (Graphics& g)
{
}

MainContentComponent::MainContentComponent()
{
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
    centreControl.setSliderStyle(Slider::Rotary);
    getLookAndFeel().setColour(Slider::trackColourId, Colours::darkgrey);
}

void MainContentComponent::resized()
{
    int morphySize = getWidth()*0.6f;
    int listenerSize = morphySize * 0.2f;
    int listenerPosition = (morphySize/2.0f) - (listenerSize/2.0f);

    listener.setBounds(listenerPosition, listenerPosition, listenerSize, listenerSize);
    angleShown.setBounds(getWidth()*0.27f, getHeight()*0.16f, getWidth()*0.8f, getHeight()*0.8f);

    azimuthControl.setBounds(getWidth()*0.14f, getHeight()*0.3f, getWidth()*0.2f, getHeight()*0.6f);
    centreControl.setBounds(getWidth()*0.1f, getHeight()*0.1f, getWidth()*0.3f, getHeight()*0.3f);
}

void MainContentComponent::paint(Graphics& g)
{
     g.drawImageAt(background, 0, 0);
}

MainContentComponent::~MainContentComponent()
{
    setLookAndFeel(nullptr);
} 