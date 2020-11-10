/*
  ==============================================================================

    Border.cpp
    Created: 10 Nov 2020 10:36:21pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "Border.h"

Border::Border()
{
    box.addRoundedRectangle(getLocalBounds(), 0.5f);
    setOn();
}

void Border::resized()
{
    box.clear();
    box.addRoundedRectangle(getLocalBounds(), 0.5f);
}

void Border::paint(Graphics& g)
{
    g.setColour(borderColour);
    g.strokePath(box, PathStrokeType(2.f, PathStrokeType::JointStyle::curved, PathStrokeType::EndCapStyle::butt));
}

void Border::setOn()
{
    borderColour = juce::Colours::cyan;
    repaint();
}

void Border::setOff()
{
    borderColour = juce::Colours::black;
    repaint();
}
