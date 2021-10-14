/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "StoALookAndFeel.h"
#include "GUI/Head.h"
#include "GUI/AzimuthView.h"
#include "GUI/Border.h"
#include "GUI/SliderBoxes.h"

class StereoToAmbiAudioProcessorEditor : public AudioProcessorEditor, juce::Timer
{
public:
	StereoToAmbiAudioProcessorEditor(StereoToAmbiAudioProcessor&);
    ~StereoToAmbiAudioProcessorEditor();

    
    StoALookAndFeel laf;
    //==============================================================================
	void paint(Graphics& g) override;
    void resized() override;
    
    RowOfSlidersWithDial azimuthControls;
    
    AzimuthView angleShown;
    Head listener;
    Image background;
    
    RowOfSlidersWithButton deverbControls;
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    StereoToAmbiAudioProcessor& processor;
    
    juce::Label inputChannelCount;
    juce::Label outputChannelCount;
    juce::Label orderLabel;
    void updateChannelCountInformation();
    void timerCallback() override;
    
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> widthValue;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> offsetValue;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> deverbButton;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> deverbThresholdValue;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> deverbSustainValue;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoToAmbiAudioProcessorEditor);
};
