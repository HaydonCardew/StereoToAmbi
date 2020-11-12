/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GUI/Assets.h"
#include "Tools.h"

//==============================================================================

StereoToAmbiAudioProcessorEditor::StereoToAmbiAudioProcessorEditor (StereoToAmbiAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize(600, 432);
    addAndMakeVisible(mainContentComponent);
    widthValue = make_unique<AudioProcessorValueTreeState::SliderAttachment> (processor.valueTree, WIDTH_ID, mainContentComponent.spread); // this and the addAndMakeVisible() is fucking the gui. remove link to angle shown and it helps...
    offsetValue = make_unique<AudioProcessorValueTreeState::SliderAttachment> (processor.valueTree, OFFSET_ID, mainContentComponent.direction);
    deverbButton = make_unique<AudioProcessorValueTreeState::ButtonAttachment>(processor.valueTree, DEVERB_ID, mainContentComponent.deverb);
    deverbCutoffValue = make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.valueTree, DEVERB_CUTOFF_ID, mainContentComponent.deverbCutoff);
    deverbSlewrateValue = make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.valueTree, DEVERB_SLEWRATE_ID, mainContentComponent.deverbSlewrate);
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
{
    setLookAndFeel(&laf);
    
    background = ImageCache::getFromMemory(Assets::Background_png, Assets::Background_pngSize);
    
    setSize(background.getWidth(), background.getHeight());
    
    addAndMakeVisible(angleShown);
    
    addAndMakeVisible(directionBorder);
    
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

    addAndMakeVisible(listener);
    
    addAndMakeVisible(deverbBorder);
    deverb.setClickingTogglesState(true);
    deverb.setButtonText("Extract Reverb");
    deverb.setColour(TextButton::textColourOffId, Colours::black);
    deverb.setColour(TextButton::textColourOnId, Colours::black);
    deverb.setColour(TextButton::buttonColourId, Colours::darkgrey);
    deverb.setColour(TextButton::buttonOnColourId, Colours::cyan);
    deverb.changeWidthToFitText();
    deverb.onStateChange = [this] {
        if (deverb.getToggleState())
        {
            deverbBorder.setOn();
        }
        else
        {
            deverbBorder.setOff();
        }
    };
    deverb.setState(juce::Button::buttonDown);
    addAndMakeVisible(deverb);
    
    deverbCutoff.setSliderStyle(juce::Slider::LinearVertical);
    deverbCutoff.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 0.0f, 0.0f);
    deverbCutoffLabel.setText("Threshold", juce::dontSendNotification);
    deverbCutoffLabel.attachToComponent(&deverbCutoff, true);
    deverbCutoffLabel.setColour (juce::Label::textColourId, juce::Colours::grey);
    addAndMakeVisible(deverbCutoff);
    
    deverbSlewrate.setSliderStyle(juce::Slider::LinearVertical);
    deverbSlewrate.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 0.0f, 0.0f);
    deverbSlewrateLabel.setText("Release", juce::dontSendNotification);
    deverbSlewrateLabel.setColour (juce::Label::textColourId, juce::Colours::grey);
    deverbSlewrateLabel.attachToComponent(&deverbSlewrate, true);
    addAndMakeVisible(deverbSlewrate);
    
    deverbSlewrate.setAlwaysOnTop(true);
    deverbCutoff.setAlwaysOnTop(true);
    
    resized();
}

void MainContentComponent::resized()
{
    float listenerSize = 0.14764f;
    
    listener.setBoundsRelative(0.60, 0.413713, listenerSize, listenerSize);
    
    float angleShownSize = 0.762392f;
    
    angleShown.setBoundsRelative(0.379, 0.11, angleShownSize, angleShownSize);
    
    direction.setBoundsRelative(0.22519f, 0.19f, 0.366223f, 0.241731f);
    
    spread.setBoundsRelative(0.25064f, 0.353489f, 0.0866108f, 0.467847f);
    
    directionBorder.setBoundsRelative(0.233809f, 0.135913, 0.12f, 0.693495f);
    
    deverb.setBoundsRelative(0.0637715, 0.217803, 0.1, 0.1);

    deverbCutoff.setBoundsRelative(0.0269193f, 0.353489f, 0.0866108f, 0.467847f);
    
    deverbSlewrate.setBoundsRelative(0.111752f, 0.353489f, 0.0866108f, 0.467847f);
    
    deverbBorder.setBoundsRelative(0.02252, 0.135913, 0.18454, 0.69292);
    
    deverbCutoffLabel.setBoundsRelative(0.0215757f, 0.564385f, 0.107804f, 0.467847f);
    
    deverbSlewrateLabel.setBoundsRelative(0.123309f, 0.564385f, 0.0841174f, 0.467847f);
}

void MainContentComponent::paint(Graphics& g)
{
     g.drawImageAt(background, 0, 0);
}

MainContentComponent::~MainContentComponent()
{
    setLookAndFeel(nullptr);
} 
