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
    widthValue = make_unique<AudioProcessorValueTreeState::SliderAttachment> (processor.valueTree, WIDTH_ID, *mainContentComponent.azimuthControls.getSlider("Width")); // this and the addAndMakeVisible() is fucking the gui. remove link to angle shown and it helps...
    offsetValue = make_unique<AudioProcessorValueTreeState::SliderAttachment> (processor.valueTree, OFFSET_ID, *mainContentComponent.azimuthControls.getDial());
    deverbButton = make_unique<AudioProcessorValueTreeState::ButtonAttachment>(processor.valueTree, DEVERB_ID, *mainContentComponent.deverbControls.getButton());
    deverbThresholdValue = make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.valueTree, DEVERB_THRESHOLD_ID, *mainContentComponent.deverbControls.getSlider("Threshold"));
    deverbSustainValue = make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.valueTree, DEVERB_SUSTAIN_ID, *mainContentComponent.deverbControls.getSlider("Sustain"));
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
    : deverbControls({ "Threshold", "Sustain" }, "Reverb Extraction"), azimuthControls({"Width"})
{
    setLookAndFeel(&laf);
    
    background = ImageCache::getFromMemory(Assets::Background_png, Assets::Background_pngSize);
    
    setSize(background.getWidth(), background.getHeight());
    
    addAndMakeVisible(angleShown);
    
    //addAndMakeVisible(directionBorder);
    
    //addAndMakeVisible(spread);
    //spread.setRange(0.0f, 3.1416f);
    azimuthControls.getSlider("Width")->onValueChange = [this] {
        angleShown.changeAzimuth(Tools::toRadians(azimuthControls.getSlider("Width")->getValue()));
    };
    azimuthControls.getSlider("Width")->setRange(0.0, 360.0, 1.0);
    //azimuthControls.getSlider("Spread")->setPopupDisplayEnabled(true, true, nullptr);
    
    azimuthControls.getDial()->setRange(0.0, 360.0, 1.0);
    azimuthControls.getDial()->onValueChange = [this] {
        listener.rotateBy(Tools::toRadians(azimuthControls.getDial()->getValue()));
        angleShown.changeOffset(Tools::toRadians(azimuthControls.getDial()->getValue()));
    };
    azimuthControls.getDial()->setRotaryParameters(0.f, 6.282f, false);
    azimuthControls.setSuffix("°");
    azimuthControls.getDial()->setTextValueSuffix(string("°"));
    azimuthControls.setDecimalPlaces(0);
    //direction.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0.0f, 0.0f);

    addAndMakeVisible(listener);
    addAndMakeVisible(azimuthControls);
    
    deverbControls.setSuffix("%");
    //deverbControls.setDecimalPlaces(0); // use when I've moved it to display correct values
    //deverbControls.getSlider("Threshold")->setRange(0.0, 100.0, 1.0);
    //deverbControls.getSlider("Cutoff")->setRange(0.0, 100.0, 1.0);
    
    addAndMakeVisible(deverbControls);
    resized();
}

void MainContentComponent::resized()
{
    float listenerSize = 0.14764f;
    
    listener.setBoundsRelative(0.60, 0.413713, listenerSize, listenerSize);
    
    float angleShownSize = 0.762392f;
    
    angleShown.setBoundsRelative(0.379, 0.11, angleShownSize, angleShownSize);
    
    float controlLength = 0.8;
    float controlHeight = 0.08;
    azimuthControls.setBoundsRelative(0.217187, controlHeight, 0.136062, controlLength);
    
    deverbControls.setBoundsRelative(0.0135638, controlHeight, 0.187299, controlLength);
}

void MainContentComponent::paint(Graphics& g)
{
     g.drawImageAt(background, 0, 0);
}

MainContentComponent::~MainContentComponent()
{
    setLookAndFeel(nullptr);
} 
