/*
  ==============================================================================

    Head.h
    Created: 10 Nov 2020 10:35:28pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class Head : public Component
{
public:
    Head ();
    void rotateBy(float inputAngle);
    void paint(Graphics& g) override;
    
private:
    Image image;
    float angle;
};
