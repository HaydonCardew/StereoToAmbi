/*
  ==============================================================================

    SliderBoxes.h
    Created: 16 Jan 2021 11:08:40pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
#include "../StoALookAndFeel.h"
#include "Border.h"

using namespace std;

class RowOfSliders : public Component
{
public:
    RowOfSliders(vector<string> labelNames, float relativeSliderHeight = 1.0);
    ~RowOfSliders();
    
    void constructSliders();
    void resized() override;
    
    void setSuffix(string suffix);
    void setDecimalPlaces(int numberOfPlaces);
    void setEnabled(bool state);
    
    shared_ptr<Slider> getSlider(string name);
    vector<shared_ptr<Slider>> sliders;
    vector<shared_ptr<Label>> labels;
    vector<string> labelNames;
    Border border;
    StoALookAndFeel laf;
    float sliderHeight;
    bool showBorder;
};

class RowOfSlidersWithDial : public RowOfSliders
{
public:
    RowOfSlidersWithDial(vector<string> labelNames);
    RowOfSlidersWithDial();
    void resized() override;
    shared_ptr<Slider> getDial();
    shared_ptr<Slider> dial;
};

class RowOfSlidersWithButton : public RowOfSliders
{
    public:
    RowOfSlidersWithButton(vector<string> labelNames, string buttonText);
    RowOfSlidersWithButton();
    void constructButton();
    void resized() override;
    shared_ptr<TextButton> getButton();
    shared_ptr<TextButton> button;
};
