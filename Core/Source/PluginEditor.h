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

#include <vector>
#include <string>

class Human : public Component {
    public:
    Human() {
		classPaths.push_back(&head);
		classPaths.push_back(&rightEar);
		classPaths.push_back(&leftEar);
		classPaths.push_back(&nose);
		angle = 0;
		size = 100;
		resized();
    };

	Path head;
	Path rightEar;
	Path leftEar;
	Path nose;
	std::vector<Path*> classPaths;

	float angle;
	float size;

    void draw(){
		clearAllPaths();

		float headSize = 0.8f * size;
		float offset = (size-headSize) / 2.0f;
        head.addEllipse(offset, offset, headSize, headSize);

        float earLength = headSize * 0.3f;
        float earWidth = earLength * 0.5f;

        rightEar.addRoundedRectangle((size-offset) - earWidth/ 2.0f, size/ 2.0f - earLength/ 2.0f, earWidth, earLength, earLength*0.25f);

        leftEar.addRoundedRectangle(offset - earWidth/ 2.0f, size/ 2.0f - earLength/ 2.0f, earWidth, earLength, earLength*0.25f);

        // add nose
        float noseLength = size*0.1f;
        float noseWidth = size*0.3f;
        juce::Point<float> top(size/ 2.0f,-noseLength + offset);
        juce::Point<float> left((size/ 2.0f)-(noseWidth/ 2.0f), noseLength + offset);
        juce::Point<float> right((size/ 2.0f)+(noseWidth/ 2.0f), noseLength + offset);
        nose.addTriangle(top, left, right);

        rotate();

		repaint();
    }

    void clearAllPaths(){
        for (int i = 0; i < classPaths.size(); i++) {
			classPaths[i]->clear();
		}
    }

	void rotateBy(float inputAngle){
	    angle = inputAngle;
	    draw();
	}

	void rotate() {
		for (int i = 0; i < classPaths.size(); ++i) {
			classPaths[i]->applyTransform(AffineTransform::rotation(angle, size/ 2.0f, size/ 2.0f));
		}
	}

    void resized() override {
		size = static_cast<float>( (getLocalBounds().getHeight() > getLocalBounds().getWidth()) ? getLocalBounds().getWidth() : getLocalBounds().getHeight() );
		draw();
    }

    void translateToOrigin(Path &object) {
		Rectangle<int> outline = getLocalBounds();
		object.applyTransform(AffineTransform::translation(static_cast<float>(-outline.getX()), static_cast<float>(-outline.getY())));
	}

    void paint(Graphics& g) override {
		g.setColour (Colours::white);
		for (int i = 0; i < classPaths.size(); i++) {
			g.fillPath(*classPaths[i]);
		}
    }
};

class AzimuthView : public Component
{
public:
    AzimuthView() : rads(0), offset(0)
    {
        drawPie();
    };

    void changeAzimuth(float azi)
    {
        azi /= 2;
        if (rads != azi && 6.282 >= azi && azi >= 0.f)
        {
            rads = azi;
            drawPie();
        }
    }

    void changeOffset(float azi)
    {
        if (offset != azi && 6.282 >= azi && azi >= 0.f)
        {
            offset = azi;
            drawPie();
        }
    }

    void paint(Graphics& g) override
    {
        g.setColour(juce::Colour (0, 255, 255).withAlpha(0.5f));
        g.fillPath(segment);
    }

    void resized() override
    {
        drawPie();
    }
    
private:
    float rads;
    float offset;
    Path segment;

    void drawPie()
    {
        segment.clear();
        int size = getSquareSize();
        segment.addPieSegment(0.0f, 0.0f, size, size, -rads+offset, rads+offset, 0.0f);
        repaint();
    }
    
    int getSquareSize()
    {
        return getWidth() > getHeight() ? getHeight(): getWidth();
    }
};

class RowOfButtons : public Component
{
public:
	RowOfButtons(std::vector<std::string> buttons, int radioGroupID=12345)
    : nButtons((unsigned)buttons.size())
    {
        for (int i = 0; i < nButtons; ++i)
		{
			auto* tb = addToList(new TextButton(buttons[i]));

			tb->setClickingTogglesState(true);
			tb->setRadioGroupId(radioGroupID);
			tb->setColour(TextButton::textColourOffId, Colours::black);
			tb->setColour(TextButton::textColourOnId, Colours::black);
			tb->setColour(TextButton::buttonColourId, Colours::darkgrey);
			tb->setColour(TextButton::buttonOnColourId, Colours::purple);
			tb->setConnectedEdges(((i != 0) ? Button::ConnectedOnLeft : 0) | ((i != (nButtons - 1)) ? Button::ConnectedOnRight : 0));

			if (i == 0)
            {
				tb->setToggleState(true, dontSendNotification);
			}
		}
	}
    
	void resized()
    {
		int buttonWidth = getWidth() / nButtons;
		for (int i = 0; i < components.size(); i++)
        {
			components[i]->setBounds(0 + (i*buttonWidth), 0, buttonWidth, getHeight());
		}
	}
private:
	int nButtons;
    
	OwnedArray<Component> components;
	template <typename ComponentType>ComponentType* addToList(ComponentType* newComp)
	{
		components.add(newComp);
		addAndMakeVisible(newComp);
		return newComp;
	}
};

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
    AzimuthView angleShown;
    Human listener;
    Image background;
    ToggleButton deverb;
    Slider deverbCutoff;
    Slider deverbSlewrate;
	
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
