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
    dial = ImageCache::getFromMemory (Assets::Slider_png, Assets::Slider_pngSize);
    rotaryKnob = ImageCache::getFromMemory (Assets::Dial_png, Assets::Dial_pngSize);
    shadow = ImageCache::getFromMemory (Assets::Shadow_png, Assets::Shadow_pngSize);
}

void StoALookAndFeel::drawLinearSlider(Graphics & g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider & slider)
{
    float newMaxSliderPos = maxSliderPos - dial.getHeight();
    float scale = newMaxSliderPos / maxSliderPos;
    g.drawImageAt(dial, 0, sliderPos*scale);
}

void StoALookAndFeel::drawRotarySlider(Graphics & g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider & slider)
{   
    g.drawImageAt(shadow, 0, 0);
    float angle = sliderPosProportional * 2 * 3.141;
    g.drawImageTransformed(rotaryKnob, AffineTransform::rotation(angle, rotaryKnob.getWidth()/2, rotaryKnob.getHeight()/2));
}
