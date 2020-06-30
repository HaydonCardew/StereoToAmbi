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
    
    addAndMakeVisible(spread);
    spread.setRange(0.0f, 3.1416f);
    spread.onValueChange = [this] {
        angleShown.changeAzimuth(spread.getValue()); 
    };
    spread.setSliderStyle(juce::Slider::LinearVertical);
    spread.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0.0f, 0.0f);
    
    addAndMakeVisible(direction);
    direction.setSliderStyle(Slider::Rotary);
    direction.setRange(0.0f, 3.1416f * 2.0f);
    direction.onValueChange = [this] {
        listener.rotateBy(direction.getValue());
        angleShown.changeCentrePosition(direction.getValue());
    };
    direction.setRotaryParameters(0.f, 6.282f, false);
    direction.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0.0f, 0.0f);
    
    addAndMakeVisible(orderSelect);
    addAndMakeVisible(listener);
    
    resized();
}

void MainContentComponent::resized()
{
    float listenerSize = 0.14764f;
    listener.setBoundsRelative(0.452, 0.43927, listenerSize, listenerSize);
    float angleShownSize = 0.762392f;
    angleShown.setBoundsRelative(0.278609, 0.120073, angleShownSize, angleShownSize);
    
    direction.setBoundsRelative(0.0965741f, 0.0915974f, 0.155535f, 0.306973f);
    spread.setBoundsRelative(0.137192f, 0.382995f, 0.3f, 0.52f);
    orderSelect.setBoundsRelative(0.76f, 0.165f, 0.2, 0.1f);
}

void MainContentComponent::paint(Graphics& g)
{
     g.drawImageAt(background, 0, 0);
}

MainContentComponent::~MainContentComponent()
{
    setLookAndFeel(nullptr);
} 
