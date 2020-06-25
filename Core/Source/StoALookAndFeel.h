/*
  ==============================================================================

    StoALookAndFeel.h
    Created: 13 Jun 2020 1:26:34am
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class StoALookAndFeel : public LookAndFeel_V3
{
private:
    Image dial;
    
public:
    StoALookAndFeel();
    
    void drawLinearSlider (Graphics&, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider&) override;
};
