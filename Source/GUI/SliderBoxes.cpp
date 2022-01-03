/*
  ==============================================================================

    SliderBoxes.cpp
    Created: 16 Jan 2021 11:08:40pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "SliderBoxes.h"

RowOfSliders::RowOfSliders(vector<string> labelNames, float relativeSliderHeight) : labelNames(labelNames), sliderHeight(relativeSliderHeight), showBorder(true)
{
    setLookAndFeel(&laf);
    constructSliders();
}

RowOfSliders::~RowOfSliders()
{
    setLookAndFeel(nullptr);
}

void RowOfSliders::constructSliders()
{
    sliders.clear();
    labels.clear();
    for (unsigned i = 0; i < labelNames.size(); ++i)
    {
        sliders.push_back(make_shared<Slider>());
        sliders[i]->setSliderStyle(juce::Slider::LinearVertical);
        sliders[i]->setTextBoxIsEditable(true);
        sliders[i]->setAlwaysOnTop(true);
        sliders[i]->setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
        
        labels.push_back(make_shared<Label>());
        labels[i]->setText(labelNames[i], juce::dontSendNotification);
        labels[i]->setColour(juce::Label::textColourId, juce::Colours::grey);
        labels[i]->setJustificationType(juce::Justification::bottomLeft);
        labels[i]->setMinimumHorizontalScale(1.0f);
        
        addAndMakeVisible(*sliders[i]);
        addAndMakeVisible(*labels[i]);
    }
    if (showBorder)
    {
        addAndMakeVisible(border);
    }
}

void RowOfSliders::resized()
{
    const int nSliders = static_cast<int>(sliders.size());
    float increment = 1.f/nSliders;
    float x = 0;
    for (unsigned i = 0; i < nSliders; ++i)
    {
        sliders[i]->setBoundsRelative(x, 1.f-sliderHeight, increment, sliderHeight);
        float width = labels[i]->getFont().getStringWidth(labels[i]->getText());
        float widthRelative = width/getWidth();
        labels[i]->setBoundsRelative(x+(increment/2)-(widthRelative/2), 0, widthRelative*2, 1.f-sliderHeight);
        x += increment;
    }
    border.setBounds(getLocalBounds());
}

shared_ptr<Slider> RowOfSliders::getSlider(string name)
{
    //assert(sliders.size() == labelNames.size()); // use map?
    for ( int i = 0; i < labels.size(); ++i )
    {
        if ( labelNames[i] == name )
        {
            return sliders[i];
        }
    }
    return nullptr;
}

void RowOfSliders::setSuffix(string suffix)
{
    for ( auto & slider : sliders)
    {
        slider->setTextValueSuffix(suffix);
    }
}

void RowOfSliders::setDecimalPlaces(int numberOfPlaces)
{
    for ( auto & slider : sliders)
    {
        slider->setNumDecimalPlacesToDisplay(numberOfPlaces);
    }
}

void RowOfSliders::setEnabled(bool state)
{
    for ( auto & slider : sliders)
    {
        slider->setEnabled(state);
    }
}

RowOfSlidersWithDial::RowOfSlidersWithDial(vector<string> labelNames) : RowOfSliders(labelNames, 0.7), dial(make_shared<Slider>())
{
    dial->setSliderStyle(Slider::Rotary);
    dial->setAlwaysOnTop(true);
    dial->setColour(Slider::textBoxOutlineColourId, Colours::transparentBlack);
    addAndMakeVisible(*dial);
}

void RowOfSlidersWithDial::resized()
{
    RowOfSliders::resized();
    const float scaling = 0.75;
    const int start = getWidth() * (1-scaling)/2;
    const int size = scaling * getWidth();
    dial->setBounds (start, 5, size, getWidth());
}

shared_ptr<Slider> RowOfSlidersWithDial::getDial()
{
    return dial;
}

RowOfSlidersWithButton::RowOfSlidersWithButton(vector<string> labelNames, string buttonText) : RowOfSliders(labelNames, 0.7)
{
    button = make_shared<TextButton>(buttonText);
    constructButton();
}
RowOfSlidersWithButton::RowOfSlidersWithButton() : RowOfSliders({"First", "Second"}, 0.8)
{
    button = make_shared<TextButton>("Test");
    constructButton();
}
void RowOfSlidersWithButton::constructButton()
{
    button->setClickingTogglesState(true);
    button->setColour(TextButton::textColourOffId, Colours::black);
    button->setColour(TextButton::textColourOnId, Colours::black);
    button->setColour(TextButton::buttonColourId, Colours::darkgrey);
    button->setColour(TextButton::buttonOnColourId, Colours::cyan);
    button->onStateChange = [this] {
        if (button->getToggleState())
        {
            border.setOn();
            setEnabled(true);
        }
        else
        {
            border.setOff();
            setEnabled(false);
        }
    };
    button->setState(juce::Button::buttonDown);
    button->setAlwaysOnTop(true);
    addAndMakeVisible(*button);
}

void RowOfSlidersWithButton::resized()
{
    RowOfSliders::resized();
    float buttonWidth = 0.7;
    float buttonHeight = 0.4;
    const float heightForButton = 1.0 - sliderHeight;
    const float buttonHeightPos = 0.05;
    button->setBoundsRelative( (1.0 - buttonWidth)/2.0, buttonHeightPos, buttonWidth, heightForButton * buttonHeight );
}

shared_ptr<TextButton> RowOfSlidersWithButton::getButton()
{
    return button;
}
