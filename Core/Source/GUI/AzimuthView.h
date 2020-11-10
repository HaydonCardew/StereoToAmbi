/*
  ==============================================================================

    AzimuthView.h
    Created: 10 Nov 2020 10:36:37pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class AzimuthView : public Component
{
public:
    AzimuthView();
    void changeAzimuth(float azi);
    void changeOffset(float azi);
    void paint(Graphics& g) override;
    void resized() override;
    
private:
    float rads;
    float offset;
    Path segment;
    void drawPie();
    int getSquareSize();
};
