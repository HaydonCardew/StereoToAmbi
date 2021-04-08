/*
  ==============================================================================

    StoALookAndFeel.cpp
    Created: 13 Jun 2020 1:26:34am
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "StoALookAndFeel.h"
#include "GUI/Assets.h"

StoALookAndFeel::StoALookAndFeel()
{
    dial = ImageCache::getFromMemory (Assets::Slider_png, Assets::Slider_pngSize);
    rotaryKnob = ImageCache::getFromMemory (Assets::Dial_png, Assets::Dial_pngSize);
    shadow = ImageCache::getFromMemory (Assets::Shadow_png, Assets::Shadow_pngSize);
    float scaling = 0.75;
    rotaryKnob = rotaryKnob.rescaled(rotaryKnob.getWidth() * scaling, rotaryKnob.getHeight() * scaling, Graphics::ResamplingQuality::highResamplingQuality);
    shadow = shadow.rescaled(shadow.getWidth() * scaling, shadow.getHeight() * scaling, Graphics::ResamplingQuality::highResamplingQuality);
}

void StoALookAndFeel::drawLinearSlider(Graphics & g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider & slider)
{
    //g.drawRect(0, 0, width, height);
    //g.drawRect(0, 0, x, y);
    const int lineThickness = 2;
    int dialWidth = dial.getWidth();
    /*if ( dialWidth > width )
    {
        dialWidth = width;
    }*/
    const int dialHeight = dial.getHeight();
    const int maxDialHeight = height - dialHeight;
    g.fillRect((width-lineThickness)/2, dialHeight/2, lineThickness, height);
    g.drawImageAt(dial, (width-dialWidth)/2.f, sliderPos-(dialHeight/2));
    //g.drawImageAt(dial, 0, 0);
    //float scale = maxDialHeight * (sliderPos / maxSliderPos);
    //g.drawImageAt(dial, 0, minSliderPos);
    //g.drawImageAt(dial, 0, maxSliderPos);
    
    //g.drawImageAt(dial, 0, maxDialHeight);
    // max - min
    //int scale = ;
    //g.fillRect((dialWidth/2)-(lineThickness/2), dialHeight/2, lineThickness, maxDialHeight);
    //g.fillRect((dialWidth/2)-(lineThickness/2), (int)minSliderPos, lineThickness, int(maxSliderPos - minSliderPos));
    //float newMaxSliderPos = maxSliderPos;// - dialHeight;
}

void StoALookAndFeel::drawRotarySlider(Graphics & g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider & slider)
{   
    const float knobWidth = rotaryKnob.getWidth();
    const float knobHeight = rotaryKnob.getHeight();
    if (height < knobHeight)
    {
        height = knobHeight;
    }
    float shadowOffset = 5;
    g.drawImageAt(shadow, ((width-shadow.getWidth())/2) + shadowOffset, ((height-shadow.getHeight())/2) + shadowOffset);
    float angle = sliderPosProportional * 2 * 3.141;
    AffineTransform transform (
                               AffineTransform::rotation ( angle, knobWidth/2, knobHeight/2 )
                               .followedBy( AffineTransform::translation((width-knobWidth)/2, (height-knobHeight)/2) )
                               );
    g.drawImageTransformed(rotaryKnob, transform);
}

Slider::SliderLayout StoALookAndFeel::getSliderLayout( Slider & slider)
{
    Rectangle<int> bounds = slider.getBounds();
    const int width = bounds.getWidth();
    const int height = bounds.getHeight();
    juce::Slider::SliderLayout layout;
    if (slider.isVertical())
    {
        int sliderHeight = dial.getHeight();
        Rectangle<float> sliderPos(0.0, 0.0);
        Rectangle<float> sliderSize(1.0, 0.95);
        Rectangle<float> textPos(0.0, 0.9);
        Rectangle<float> textSize(1.0, 0.1);
        layout.sliderBounds = { int(sliderPos.getWidth() * width), int(sliderPos.getHeight() * height) + sliderHeight/2,
                                int(sliderSize.getWidth() * width), int(sliderSize.getHeight() * height) - sliderHeight};
        layout.textBoxBounds = { int(textPos.getWidth() * width), int(textPos.getHeight() * height),
                                 int(textSize.getWidth() * width), int(textSize.getHeight() * height) };
    }
    else {
        assert(slider.isRotary());
        int knobHeight = rotaryKnob.getHeight();
        int knobWidth = rotaryKnob.getWidth();
        if (knobWidth > width)
        {
            knobWidth = width;
        }
        layout.sliderBounds = { width/2, 0, knobWidth, knobHeight};
        layout.textBoxBounds = { 0, int(knobHeight*1.1), knobWidth, int(knobHeight*0.2)};
    }
    return layout;
}
