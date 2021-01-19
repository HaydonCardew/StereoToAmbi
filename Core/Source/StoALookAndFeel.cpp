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
    int textHeight = dial.getHeight() / 2.0;
    int textWidth = width * 0.6;
    layout.sliderBounds = { 0, dial.getHeight()/2, width, height - dial.getHeight() - textHeight };
    layout.textBoxBounds = { (width-textWidth)/2, height-textHeight, textWidth, textHeight };
    return layout;
}
