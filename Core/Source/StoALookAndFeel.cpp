/*
  ==============================================================================

    StoALookAndFeel.cpp
    Created: 13 Jun 2020 1:26:34am
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "StoALookAndFeel.h"
#include "Assets.h"

StoALookAndFeel::StoALookAndFeel()
{
    dial = ImageCache::getFromMemory (Assets::Dial_png, Assets::Dial_pngSize);
    rotaryKnob = ImageCache::getFromMemory (Assets::Knob_png, Assets::Knob_pngSize);
}

void StoALookAndFeel::drawLinearSlider(Graphics & g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider & slider)
{
    
    g.drawImage(dial, 0, sliderPos, dial.getWidth(), dial.getHeight(),
                0, 0, dial.getWidth(), dial.getHeight());
}

void StoALookAndFeel::drawRotarySlider(Graphics & g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider & slider)
{   
    float angle = sliderPosProportional * 2 * 3.141;
    g.drawImageTransformed(rotaryKnob, AffineTransform::rotation(angle, rotaryKnob.getWidth()/2, rotaryKnob.getHeight()/2));
}
