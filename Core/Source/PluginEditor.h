/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

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

class MorphedRectangularGrid : public Component {
public:
	MorphedRectangularGrid(const float size = 300.0f, float noOfLines = 17.0f, float centreProportion = 0.8f, float curveProportion = 0.6f)
		: noOfLines(noOfLines), centreProportion(centreProportion), curveProportion(curveProportion)
	{
		draw(size);
	}

	void resized() override {
		visual.clear();
		float size = static_cast<float>( (getLocalBounds().getHeight() > getLocalBounds().getWidth()) ? getLocalBounds().getWidth() : getLocalBounds().getHeight() );
		//float innerSize = size / 1.8;
		draw(size);
	}

	Path visual;
    float pi = 3.14159f;
	const float noOfLines;
	const float centreProportion;
	const float curveProportion;

	float getMaxCurve(float size) {
		float length = size;
		return sqrt(pow(length, 2)/2) - (length / 2.0f);
	}

	void translateToOrigin(Path &object) {
		Rectangle<float> outline = object.getBounds();
		object.applyTransform(AffineTransform::translation(-outline.getX()/1.5f, -outline.getY()/1.5f)); //no idea why 1.5
	}

	void duplicateAndRotate90Degrees(Path &object) {
		Path mirror = object;
		Rectangle<float> bounds = mirror.getBounds();
		mirror.applyTransform(AffineTransform::rotation(pi/2.0f, bounds.getCentreX(), bounds.getCentreY()));
		object.addPath(mirror);
	}

	float getOffset(float lineNumber, float noOfLines, float curveOffset) {
		// first line is '0', second '1'...
		float alpha = lineNumber / (noOfLines - 1);
		alpha = (2.0f * alpha) - 1.0f;
		return curveOffset * alpha;
	}

	Path getArcWithLinkedLines(float centre, float radius, float angle, float rotationOfArc, std::vector<juce::Point<float>> linkedPoints, bool reverse=false) {
		Path arc;
		arc.addCentredArc(centre, centre, radius, radius, rotationOfArc, -angle, angle, true);
		size_t noOfLines = linkedPoints.size();
		float stepSize = arc.getLength() / (static_cast<float>(noOfLines) - 1.0f);
		std::vector<juce::Point<float>> arcPoints;
		for (int lineNumber = 0; lineNumber < noOfLines; lineNumber++)
		{
			arcPoints.push_back(arc.getPointAlongPath(lineNumber * stepSize));
		}

		// reverse when linkedPoints go l-to-r and the arc is underneath...
		if (reverse) {
			std::reverse(linkedPoints.begin(), linkedPoints.end());
		}

		for (int lineNumber = 0; lineNumber < noOfLines; lineNumber++) {
			arc.startNewSubPath(arcPoints[lineNumber]);
			arc.lineTo(linkedPoints[lineNumber]);
		}
		return arc;
	}

	Path getInsideGrid(const float size, std::vector<juce::Point<float>> &topGridPoints, std::vector<juce::Point<float>> &bottomGridPoints) {
		Path grid;
		topGridPoints.clear();
		bottomGridPoints.clear();
		float maxCurve = getMaxCurve(size);
		float curveOffset = maxCurve * curveProportion * 2.0f; // why by 2?!

		grid.quadraticTo(size / 2.0f, -curveOffset, size, 0.0f);
		std::vector<juce::Point<float>> midPoints;
		float stepSize = grid.getLength() / (noOfLines - 1);
		for (int lineNumber = 0; lineNumber < noOfLines; lineNumber++) {
			juce::Point<float> start = grid.getPointAlongPath(stepSize*lineNumber);
			topGridPoints.push_back(start);
			juce::Point<float> mid(start.getX() + getOffset(lineNumber, noOfLines, curveOffset), size / 2);
			midPoints.push_back(mid);
			juce::Point<float> end = start;
			end.setY(size - start.getY());
			bottomGridPoints.push_back(end);
		}

		// 'copy' top curve to bottom
		juce::Point<float> bottomLeft(0, size);
		grid.startNewSubPath(bottomLeft);
		grid.quadraticTo(size / 2, size + curveOffset, size, size);

		for (int lineNumber = 0; lineNumber < noOfLines; lineNumber++) {
			grid.startNewSubPath(topGridPoints[lineNumber]);
			grid.quadraticTo(midPoints[lineNumber], bottomGridPoints[lineNumber]);
		}
		return grid;
	}

	void draw(const float size, float innerRatio = 0.5) {

		// pass 'size' and figure the rest out...
		float insideGridSize = size * innerRatio;

		std::vector<juce::Point<float>> topGridPoints;
		std::vector<juce::Point<float>> bottomGridPoints;

		Path insideGrid = getInsideGrid(insideGridSize, topGridPoints, bottomGridPoints);
		visual.addPath(insideGrid);

		//add top arc & bottom arc
		float centre = insideGridSize / 2.0f;
		float radius = size / 2.0f;
		float angle = 0.785398f; // 45 degrees in rads;
		// change the 1.0f and make into a function to return path
		Path topArc = getArcWithLinkedLines(centre, radius, angle, 0.0f, topGridPoints);
        visual.addPath(topArc);
		Path bottomArc = getArcWithLinkedLines(centre, radius, angle, pi, bottomGridPoints, true);
        visual.addPath(bottomArc);

		//
		/*
		float a = 0.5 * sqrt(pow(radius, 2)*2);
		float z = sqrt(pow(radius, 2) - pow(a, 2));
		*/
		float maxSquareInsideCircleSize = topArc.getPointAlongPath(topArc.getLength()).getX() - topArc.getPointAlongPath(0).getX();
		float maxExcursion = getMaxCurve(maxSquareInsideCircleSize) * 2.0f;

		// this should go in the morphed arc function?
		//should be getMaxCurve(insideGridSize) + root(lengthToFirstCurvedLine^2 /2 )
		float minExcursion = getMaxCurve(insideGridSize) * 1.1f;
		float noLines = 8.0f;

		// start of arc point

		juce::Point<float> topLeft(0, 0);
		juce::Point<float> topLeftExtra = topArc.getPointAlongPath(0);
		Path tarc = getFilledMorphedArc(topLeft, topLeftExtra, insideGridSize, noLines, maxExcursion, minExcursion);
		juce::Point<float> bottomRight(insideGridSize, insideGridSize);
		juce::Point<float> bottomRightExtra = bottomArc.getPointAlongPath(0);
		visual.addPath(tarc);
		Path barc = getFilledMorphedArc(bottomRight, bottomRightExtra, insideGridSize, noLines, -maxExcursion, -minExcursion);
		visual.addPath(barc);

        duplicateAndRotate90Degrees(visual);
		translateToOrigin(visual);
	};

	Path getFilledMorphedArc(juce::Point<float> p1, juce::Point<float> p2, float length, float noOfLines, float maxExcursion, float minExcursion){
	    Path arc;
		juce::Point<float> p3, p4;

		bool midPiece = (p1.getX() > p2.getX() && p1.getY() > p2.getY()) || (p2.getX() > p1.getX() && p2.getY() > p1.getY());
		bool top = p1.getY() > p2.getY();
		bool left = p1.getX() > p2.getX();

		if (midPiece) {
			if (top) {
				float extra = p1.getX() - p2.getX();
				p4.setY(p2.getY());
				p4.setX(p2.getX() + length + extra + extra);
				p3.setY(p1.getY());
				p3.setX(p1.getX() + length);
			}
			else {
				float extra = p2.getX() - p1.getX();
				p4.setY(p2.getY());
				p4.setX(p2.getX() - length - extra - extra);
				p3.setY(p1.getY());
				p3.setX(p1.getX() - length);
			}
		}
		else {
			if (left) {
				float extra = p2.getY() - p1.getY();
				p4.setY(p2.getY() - length - extra - extra);
				p4.setX(p2.getX());
				p3.setY(p1.getY() - length);
				p3.setX(p1.getX());
			}
			else {
				float extra = p1.getY() - p2.getY();
				p4.setY(p2.getY() + length + extra + extra);
				p4.setX(p2.getX());
				p3.setY(p1.getY() + length);
				p3.setX(p1.getX());
			}
		}

		bool leftToRight = p1.getX() > p2.getX();
		bool upwards = p1.getY() > p2.getY();

		//
		//    p2                          p4
		//      \                        /
		//       \----------------------/
		//        \                    /
		//         \------------------/
		//          \                /
		//           p1------------p3
		//
		//           |----Length----|-extra-|

		Line<float> leftLine(p1, p2), rightLine(p3, p4);

		for (int i = 0; i < noOfLines; i++) {
			float distancePercentage = ((float)i + 1.0f) / (noOfLines+1.0f);
		  float distance = 2;//pow<float>(distancePercentage, 0.8f) * leftLine.getLength();

			juce::Point<float> leftPoint = leftLine.getPointAlongLine(distance);
			juce::Point<float> rightPoint = rightLine.getPointAlongLine(distance);

			// make it a % of max excursion relative to distance along the left/right line

			float excursion = ((maxExcursion-minExcursion) * distancePercentage) + minExcursion;

			juce::Point<float> midPoint(length /2.0f, leftPoint.getY() - excursion);
			arc.startNewSubPath(leftPoint);
			arc.quadraticTo(midPoint, rightPoint);
		}

		return arc;
	}

	void paint(Graphics& g) override {
		g.setColour(Colours::black);
		PathStrokeType stroke(1.0f);
		g.strokePath(visual, stroke);
	}
};

class AzimuthView : public Component {
public:
	AzimuthView(float startAzi = 1.0f, float startCentreRads = 0) {
		rads = startAzi;
		centreRads = startCentreRads;
		drawPie();
	};

	float rads;
	float centreRads;
	Path segment;

	void drawPie() {
		segment.clear();
		int size = getSquareSize();
		segment.addPieSegment(0.0f, 0.0f, size, size, -rads+centreRads, rads+centreRads, 0.0f);
		repaint();
	}

	void changeAzimuth(float azi) {
		rads = azi;
		drawPie();
	}

	void changeCentrePosition(float azi) {
		centreRads = azi;
		drawPie();
	}

	void paint(Graphics& g) override
	{
		g.setColour(Colours::purple.withAlpha(0.5f));
		g.fillPath(segment);
	}

	int getSquareSize() {
		return getWidth() > getHeight() ? getHeight(): getWidth();
	}

	void resized() override {
		drawPie();
	}
};

class AzimuthControl : public Component
{

    Slider azimuthControl;
	Slider centreControl;
    AzimuthView angleShown;
	MorphedRectangularGrid morphy;
	Human listener;

    public:
    AzimuthControl(){

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
		addAndMakeVisible(morphy);
		addAndMakeVisible(angleShown);
        addAndMakeVisible(azimuthControl);
		addAndMakeVisible(centreControl);
        addAndMakeVisible(listener);
		getLookAndFeel().setColour(Slider::trackColourId, Colours::darkgrey);

    }

    void resized() override
    {
        int morphySize = getWidth()*0.6f;
        int listenerSize = morphySize * 0.2f;
		int listenerPosition = (morphySize/2.0f) - (listenerSize/2.0f);

		listener.setBounds(listenerPosition, listenerPosition, listenerSize, listenerSize);
		morphy.setBounds(0, 0, morphySize, morphySize);
        angleShown.setBounds(0, 0, morphySize, morphySize);

		azimuthControl.setBounds(getWidth()*0.6f, getHeight()*0.2f, getWidth()*0.2f, getHeight()*0.6f);
		centreControl.setBounds(getWidth()*0.8f, getHeight()*0.2f, getWidth()*0.2f, getHeight()*0.6f);
    }

};

class RowOfButtons : public Component
{
public:
	RowOfButtons(std::vector<std::string> buttons, int radioGroupID=12345) {
		numberOfButtons = buttons.size();
		for (int i = 0; i < numberOfButtons; ++i)
		{
			auto* tb = addToList(new TextButton(buttons[i]));

			tb->setClickingTogglesState(true);
			tb->setRadioGroupId(radioGroupID);
			tb->setColour(TextButton::textColourOffId, Colours::black);
			tb->setColour(TextButton::textColourOnId, Colours::black);
			tb->setColour(TextButton::buttonColourId, Colours::darkgrey);
			tb->setColour(TextButton::buttonOnColourId, Colours::purple);
			tb->setConnectedEdges(((i != 0) ? Button::ConnectedOnLeft : 0) | ((i != (numberOfButtons - 1)) ? Button::ConnectedOnRight : 0));

			if (i == 0) {
				tb->setToggleState(true, dontSendNotification);
			}
		}
	}
	void resized() {
		int buttonWidth = getWidth() / numberOfButtons;
		for (int i = 0; i < components.size(); i++) {
			components[i]->setBounds(0 + (i*buttonWidth), 0, buttonWidth, getHeight());
		}
	}
	int numberOfButtons = 3;
	//static const std::vector<std::string> defaultButtons = {"1", "2", "3"};
	// This little function avoids a bit of code-duplication by adding a component to
	// our list as well as calling addAndMakeVisible on it..
	OwnedArray<Component> components;
	template <typename ComponentType>ComponentType* addToList(ComponentType* newComp)
	{
		components.add(newComp);
		addAndMakeVisible(newComp);
		return newComp;
	}
};

class OrderSelect : public Component
{
	public:
	OrderSelect() {
		for (int i = 0; i < noOfOrders; ++i)
		{
			auto* tb = addToList(new TextButton(String(i + 1)));

			tb->setClickingTogglesState(true);
			tb->setRadioGroupId(34567);
			tb->setColour(TextButton::textColourOffId, Colours::black);
			tb->setColour(TextButton::textColourOnId, Colours::black);
			tb->setColour(TextButton::buttonColourId, Colours::darkgrey);
			tb->setColour(TextButton::buttonOnColourId, Colours::purple);
			tb->setConnectedEdges(((i != 0) ? Button::ConnectedOnLeft : 0) | ((i != (noOfOrders - 1)) ? Button::ConnectedOnRight : 0));

			if (i == 0)
				tb->setToggleState(true, dontSendNotification);
		}
	}
	void resized() {
		int buttonWidth = getWidth() / noOfOrders;
		for (int i = 0; i < components.size(); i++) {
			components[i]->setBounds(0 + (i*buttonWidth), 0, buttonWidth, getHeight());
		}
	}
	const int noOfOrders = 3;
	// This little function avoids a bit of code-duplication by adding a component to
	// our list as well as calling addAndMakeVisible on it..
	OwnedArray<Component> components;
	template <typename ComponentType>ComponentType* addToList(ComponentType* newComp)
	{
		components.add(newComp);
		addAndMakeVisible(newComp);
		return newComp;
	}
};

//==============================================================================
/**
*/
class StereoToAmbiAudioProcessorEditor : public AudioProcessorEditor
{
public:
	StereoToAmbiAudioProcessorEditor(StereoToAmbiAudioProcessor&);
    ~StereoToAmbiAudioProcessorEditor();

    //==============================================================================
	void paint(Graphics& g) override;
	void resized() override {};

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    StereoToAmbiAudioProcessor& processor;
	AzimuthControl aziControl;
	const std::vector<std::string> orders{ "1", "2", "3" };
	RowOfButtons orderSelect;
	std::vector<std::string> channelFormats = {"FuMa", "ACN"};
	RowOfButtons channelFormatSelect;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoToAmbiAudioProcessorEditor)
};

//==============================================================================
class MainContentComponent   : public Component
{
public:
    MainContentComponent()
    {
		aziControl.setBounds(0, 0, 300, 300);
		addAndMakeVisible(aziControl);
		orderSelect.setBounds(350, 40, 200, 30);
		addAndMakeVisible(orderSelect);
        setSize (600, 600);
    }
    ~MainContentComponent() {}

	AzimuthControl aziControl;
	OrderSelect orderSelect;

    void paint (Graphics& g) override {
		g.setColour(Colours::lightgrey);
		g.fillAll();
    }

	void resized() override {};

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
