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
    : AudioProcessorEditor (&p), processor (p)
{
    setSize(600, 346);
    addAndMakeVisible(mainContentComponent);
}

void StereoToAmbiAudioProcessorEditor::resized()
{
    mainContentComponent.setBounds(0, 0, 600, 346);
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
    
    addAndMakeVisible(angleShown);
    
    addAndMakeVisible(azimuthControl);
    azimuthControl.setRange(0.0f, 3.1416f);
    azimuthControl.onValueChange = [this] {angleShown.changeAzimuth(azimuthControl.getValue()); };
    azimuthControl.setSliderStyle(juce::Slider::LinearVertical);
    azimuthControl.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0.0f, 0.0f);
    
    addAndMakeVisible(centreControl);
    centreControl.setSliderStyle(Slider::Rotary);
    centreControl.setRange(0.0f, 3.1416f * 2.0f);
    centreControl.onValueChange = [this] {
        listener.rotateBy(centreControl.getValue());
        angleShown.changeCentrePosition(centreControl.getValue());
    };
    centreControl.setRotaryParameters(0.f, 6.282f, false);
    centreControl.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0.0f, 0.0f);
    
    addAndMakeVisible(orderSelect);
    addAndMakeVisible(listener);
    
    resized();
}

void MainContentComponent::resized()
{
    listener.setBoundsRelative(0.27f, 0.16f, 0.2f, 0.2f);
    angleShown.setBoundsRelative(0.27f, 0.16f, 0.8f, 0.8f);
    azimuthControl.setBoundsRelative(0.14f, 0.3f, 0.2f, 0.6f);
    centreControl.setBoundsRelative(0.1f, 0.1f, 0.3f, 0.3f);
    orderSelect.setBoundsRelative(0.75f, 0.17f, 0.2f, 0.1f);
}

void MainContentComponent::paint(Graphics& g)
{
     g.drawImageAt(background, 0, 0);
}

MainContentComponent::~MainContentComponent()
{
    setLookAndFeel(nullptr);
} 
