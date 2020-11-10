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

class MainContentComponent : public Component
{
public:
    MainContentComponent();
    ~MainContentComponent();

	StoALookAndFeel laf;

    void paint (Graphics& g) override;

	void resized() override;

	Slider direction;
    Slider spread;
    Border directionBorder;
    
    AzimuthView angleShown;
    Head listener;
    Image background;
    TextButton deverb;
    Slider deverbCutoff;
    Label deverbCutoffLabel;
    Slider deverbSlewrate;
    Label deverbSlewrateLabel;
    Border deverbBorder;
	
private:
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};

class StereoToAmbiAudioProcessorEditor : public AudioProcessorEditor
{
public:
	StereoToAmbiAudioProcessorEditor(StereoToAmbiAudioProcessor&);
    ~StereoToAmbiAudioProcessorEditor();

    //==============================================================================
	void paint(Graphics& g) override;
    void resized() override;
    
    MainContentComponent mainContentComponent;
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    StereoToAmbiAudioProcessor& processor;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> widthValue;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> offsetValue;
    std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment> deverbButton;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> deverbCutoffValue;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> deverbSlewrateValue;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoToAmbiAudioProcessorEditor);
};
