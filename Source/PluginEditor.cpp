/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GUI/Assets.h"
#include "Processing/Tools.h"

//==============================================================================

StereoToAmbiAudioProcessorEditor::StereoToAmbiAudioProcessorEditor (StereoToAmbiAudioProcessor& p)
    : AudioProcessorEditor (&p), azimuthControls({"Width"}), deverbControls({ "Threshold", "Sustain" }, "Reverb\n Extraction"), processor (p)
{
    setLookAndFeel(&laf);
    
    background = ImageCache::getFromMemory(Assets::Background_png, Assets::Background_pngSize);
    
    setSize(background.getWidth(), background.getHeight());
    
    addAndMakeVisible(angleShown);

    azimuthControls.getSlider("Width")->onValueChange = [this] {
        angleShown.changeAzimuth(Tools::toRadians(azimuthControls.getSlider("Width")->getValue()));
    };
    azimuthControls.getSlider("Width")->setRange(0.0, 360.0, 1.0);
    
    azimuthControls.getDial()->setRange(0.0, 359.0, 1.0);
    azimuthControls.getDial()->onValueChange = [this] {
        listener.rotateBy(Tools::toRadians(azimuthControls.getDial()->getValue()));
        angleShown.changeOffset(Tools::toRadians(azimuthControls.getDial()->getValue()));
    };
    azimuthControls.getDial()->setRotaryParameters(0.f, 6.282f, false);
    azimuthControls.setSuffix("°");
    azimuthControls.getDial()->setTextValueSuffix(string("°"));
    azimuthControls.setDecimalPlaces(0);

    addAndMakeVisible(listener);
    addAndMakeVisible(azimuthControls);
    
    deverbControls.setSuffix("%");
    
    addAndMakeVisible(deverbControls);
    widthValue = make_unique<AudioProcessorValueTreeState::SliderAttachment> (processor.valueTree, WIDTH_ID, *azimuthControls.getSlider("Width"));
    offsetValue = make_unique<AudioProcessorValueTreeState::SliderAttachment> (processor.valueTree, OFFSET_ID, *azimuthControls.getDial());
    deverbButton = make_unique<AudioProcessorValueTreeState::ButtonAttachment>(processor.valueTree, DEVERB_ID, *deverbControls.getButton());
    deverbThresholdValue = make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.valueTree, DEVERB_THRESHOLD_ID, *deverbControls.getSlider("Threshold"));
    deverbSustainValue = make_unique<AudioProcessorValueTreeState::SliderAttachment>(processor.valueTree, DEVERB_SUSTAIN_ID, *deverbControls.getSlider("Sustain"));
    
    addAndMakeVisible(inputChannelCount);
    addAndMakeVisible(outputChannelCount);
    addAndMakeVisible(orderLabel);
    updateChannelCountInformation();
    orderLabel.setText("Output order: " + to_string(processor.getOutputOrder()), dontSendNotification);
    orderLabel.setColour(Label::textColourId, Colours::white);
    
    addAndMakeVisible(versionLabel);
    versionLabel.setText(string(VERSION), dontSendNotification);
    versionLabel.setColour(Label::textColourId, Colours::white);
    startTimer(100);
    resized();
}

void StereoToAmbiAudioProcessorEditor::resized()
{
    float listenerSize = 0.14764f;
    listener.setBoundsRelative (0.675, 0.445, listenerSize, listenerSize);
    
    float angleShownSize = 0.733;
    angleShown.setBoundsRelative (0.497, 0.145, angleShownSize, angleShownSize);
    
    float controlLength = 0.8;
    float controlHeight = 0.08;
    azimuthControls.setBoundsRelative (0.217187, controlHeight, 0.136062, controlLength);
    deverbControls.setBoundsRelative (0.0135638, controlHeight, 0.187299, controlLength);
    
    orderLabel.setBoundsRelative         (0.36, 0.7 , 0.6, 0.1);
    inputChannelCount.setBoundsRelative  (0.36, 0.75, 0.6, 0.1);
    outputChannelCount.setBoundsRelative (0.36, 0.8 , 0.6, 0.1);
    
    versionLabel.setBoundsRelative (0.85, 0.95, 0.3, 0.05);
}

void StereoToAmbiAudioProcessorEditor::updateChannelCountInformation()
{
    int in = processor.getTotalNumInputChannels();
    stringstream inputs;
    inputs << "Input Channels: " << in << "/" << 2;
    inputChannelCount.setText(inputs.str(), dontSendNotification);
    inputChannelCount.setColour(Label::textColourId, in >= 2 ? Colours::white : Colours::red);
    
    int required = processor.numberOfBFormatChannels();
    int out = processor.getTotalNumOutputChannels();
    stringstream outputs;
    outputs << "Output Channels: " << out << "/" << required;
    outputChannelCount.setText(outputs.str(), dontSendNotification);
    outputChannelCount.setColour(Label::textColourId, out >= required ? Colours::white : Colours::red);
}

void StereoToAmbiAudioProcessorEditor::timerCallback()
{
    updateChannelCountInformation();
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
