/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Assets.h"
#include "Tools.hpp"

//==============================================================================

StereoToAmbiAudioProcessorEditor::StereoToAmbiAudioProcessorEditor (StereoToAmbiAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize(600, 432);
    addAndMakeVisible(mainContentComponent);
    widthValue = make_unique<AudioProcessorValueTreeState::SliderAttachment> (processor.valueTree, WIDTH_ID, mainContentComponent.spread); // this and the addAndMakeVisible() is fucking the gui. remove link to angle shown and it helps...
    offsetValue = make_unique<AudioProcessorValueTreeState::SliderAttachment> (processor.valueTree, OFFSET_ID, mainContentComponent.direction);
    channelFormat = make_unique<AudioProcessorValueTreeState::ComboBoxAttachment> (processor.valueTree, FORMAT_ID, mainContentComponent.channelFormat);
}

void StereoToAmbiAudioProcessorEditor::resized()
{
    mainContentComponent.setBounds(0, 0, 600, 432);
}

StereoToAmbiAudioProcessorEditor::~StereoToAmbiAudioProcessorEditor()
{
}

//==============================================================================
void StereoToAmbiAudioProcessorEditor::paint (Graphics& g)
{
}

MainContentComponent::MainContentComponent()
//: channelOrder({"FuMa", "AmbiX"})
{
    setLookAndFeel(&laf);
    
    background = ImageCache::getFromMemory(Assets::Background_png, Assets::Background_pngSize);
    
    setSize(background.getWidth(), background.getHeight());
    
    addAndMakeVisible(angleShown);
    
    addAndMakeVisible(spread);
    //spread.setRange(0.0f, 3.1416f);
    spread.onValueChange = [this] {
        angleShown.changeAzimuth(Tools::toRadians(spread.getValue()));
    };
    spread.setSliderStyle(juce::Slider::LinearVertical);
    spread.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0.0f, 0.0f);
    
    addAndMakeVisible(direction);
    direction.setSliderStyle(Slider::Rotary);
    //direction.setRange(0.0f, 3.1416f * 2.0f);
    direction.onValueChange = [this] {
        listener.rotateBy(Tools::toRadians(direction.getValue()));
        angleShown.changeOffset(Tools::toRadians(direction.getValue()));
    };
    direction.setRotaryParameters(0.f, 6.282f, false);
    direction.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0.0f, 0.0f);
    
    channelFormat.addItem("ACN", 1);
    channelFormat.addItem("FuMa", 2);
    
    addAndMakeVisible(channelFormat);
    addAndMakeVisible(listener);
    
    resized();
}

void MainContentComponent::resized()
{
    float listenerSize = 0.14764f;
    
    listener.setBoundsRelative(0.60, 0.413713, listenerSize, listenerSize);
    
    float angleShownSize = 0.762392f;
    
    angleShown.setBoundsRelative(0.379, 0.11, angleShownSize, angleShownSize);
    
    direction.setBoundsRelative(0.073095f, 0.01181f, 0.366223f, 0.241731f);
    
    spread.setBoundsRelative(0.130751f, 0.29635f, 0.0841174f, 0.467847f);
    
    channelFormat.setBoundsRelative(0.0847174f, 0.724134f, 0.22805, 0.1f);
}

void MainContentComponent::paint(Graphics& g)
{
     g.drawImageAt(background, 0, 0);
}

MainContentComponent::~MainContentComponent()
{
    setLookAndFeel(nullptr);
} 
