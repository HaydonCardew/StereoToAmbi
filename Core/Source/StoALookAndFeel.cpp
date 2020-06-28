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
    //g.drawImageTransformed(rotaryKnob, 0, 0, rotaryKnob.getWidth(), rotaryKnob.getHeight(),
    //0, 0, rotaryKnob.getWidth(), rotaryKnob.getHeight());
    g.drawImageTransformed(rotaryKnob, AffineTransform::rotation(angle, rotaryKnob.getWidth()/2, rotaryKnob.getHeight()/2));
}

/*
 {
     g.setColour(Colours::black);
     Rectangle<int> rint(x,y,width,height);

     Rectangle<float> r = rint.toFloat().reduced(0.5f);
     g.drawRoundedRectangle(r,5,1);

     g.setColour(Colours::green);
     
     float radius = (r.getWidth()-1) / 2.f;
     
     g.drawLine(r.getX() + r.getWidth()/2.f,
                 r.getY() + radius,
                 r.getX() + r.getWidth()/2.f,
                 r.getHeight()-radius);

     auto l = (r.getHeight()-radius) - (r.getY() + radius);
     float originalPos = sliderPos / (float)height;
     
     sliderPos = l * originalPos; //scaled to our inner size
     g.setColour(Colours::white);
     
     g.fillEllipse(x + width * 0.5f - radius,
                   sliderPos,
                   radius * 2.f,
                   radius * 2.f);
     
 }
 */
