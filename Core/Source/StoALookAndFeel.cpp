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
    onDial = ImageCache::getFromMemory (Assets::BlueSlider_png, Assets::BlueSlider_pngSize);
    offDial = ImageCache::getFromMemory (Assets::OffSlider_png, Assets::OffSlider_pngSize);
    //assert(onDial.getWidth() == offDial.getWidth());
    //assert(onDial.getHeight() == offDial.getHeight());
    rotaryKnob = ImageCache::getFromMemory (Assets::BlueDial_png, Assets::BlueDial_pngSize);
    shadow = ImageCache::getFromMemory (Assets::Shadow_png, Assets::Shadow_pngSize);
    //float scaling = 0.8;// 0.75;
    //rotaryKnob = rotaryKnob.rescaled(rotaryKnob.getWidth() * scaling, rotaryKnob.getHeight() * scaling, Graphics::ResamplingQuality::highResamplingQuality);
    //shadow = shadow.rescaled(shadow.getWidth() * scaling, shadow.getHeight() * scaling, Graphics::ResamplingQuality::highResamplingQuality);
}

void StoALookAndFeel::drawLinearSlider(Graphics & g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const Slider::SliderStyle style, Slider & slider)
{
    const int lineThickness = 2;
    const int dialWidth = onDial.getWidth();
    const int dialHeight = onDial.getHeight();
    g.fillRect((width-lineThickness)/2, dialHeight/2, lineThickness, height);
    g.drawImageAt(slider.isEnabled() ? onDial : offDial, (width-dialWidth)/2.f, sliderPos-(dialHeight/2));
}

void StoALookAndFeel::drawRotarySlider(Graphics & g, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider & slider)
{
    unsigned shift = shadow.getWidth()-rotaryKnob.getWidth();
    if (rotaryKnob.getWidth() != width)
    {
        float scaling = float(width)/(shadow.getWidth()+shift);
        rotaryKnob = rotaryKnob.rescaled(rotaryKnob.getWidth() * scaling, rotaryKnob.getHeight() * scaling, Graphics::ResamplingQuality::highResamplingQuality);
        shadow = shadow.rescaled(shadow.getWidth() * scaling, shadow.getHeight() * scaling, Graphics::ResamplingQuality::highResamplingQuality);
    }
    const float knobWidth = rotaryKnob.getWidth();
    const float knobHeight = rotaryKnob.getHeight();
    float angle = sliderPosProportional * 2 * 3.141;
    AffineTransform transform (AffineTransform::rotation (angle, knobWidth/2, knobHeight/2)
                               .followedBy( AffineTransform::translation(x + shift, y) ) );
    g.drawImage(shadow, Rectangle<float>(x + shift, y, shadow.getWidth(), shadow.getHeight()));
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
        const int sliderHeight = onDial.getHeight();
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
        int textHeight = 20;
        textHeight = textHeight > bounds.getHeight() ? bounds.getHeight() : textHeight;
        layout.textBoxBounds = { 0, 0, bounds.getWidth(), textHeight };
        layout.sliderBounds  = { 0, textHeight, bounds.getWidth(), bounds.getHeight() - textHeight };
    }
    return layout;
}
