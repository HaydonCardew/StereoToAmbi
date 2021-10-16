/*
  ==============================================================================

    Border.h
    Created: 10 Nov 2020 10:36:21pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class Border : public Component
{
public:
    Border();
    void resized() override;
    void paint(Graphics& g) override;
    void setOn();
    void setOff();
private:
    juce::Colour borderColour;
    Path box;
};
