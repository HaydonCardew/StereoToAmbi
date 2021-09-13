/*
  ==============================================================================

    Head.cpp
    Created: 10 Nov 2020 10:35:28pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "Head.h"
#include "Assets.h"

Head::Head()
{
    classPaths.push_back(&head);
    classPaths.push_back(&rightEar);
    classPaths.push_back(&leftEar);
    classPaths.push_back(&nose);
    angle = 0;
    size = 100;
    resized();
    
    image = ImageCache::getFromMemory(Assets::Head_png, Assets::Head_pngSize);
}

void Head::draw()
{
    clearAllPaths();

    float headSize = 0.8f * size;
    float offset = (size-headSize) / 2.0f;
    head.addEllipse(offset, offset, headSize, headSize);

    float earLength = headSize * 0.3f;
    float earWidth = earLength * 0.5f;

    rightEar.addRoundedRectangle((size-offset) - earWidth/ 2.0f, size/ 2.0f - earLength/ 2.0f, earWidth, earLength, earLength*0.25f);

    leftEar.addRoundedRectangle(offset - earWidth/ 2.0f, size/ 2.0f - earLength/ 2.0f, earWidth, earLength, earLength*0.25f);

    // add nose
    float noseLength = size*0.1f;
    float noseWidth = size*0.3f;
    juce::Point<float> top(size/ 2.0f,-noseLength + offset);
    juce::Point<float> left((size/ 2.0f)-(noseWidth/ 2.0f), noseLength + offset);
    juce::Point<float> right((size/ 2.0f)+(noseWidth/ 2.0f), noseLength + offset);
    nose.addTriangle(top, left, right);

    rotate();

    repaint();
}

void Head::clearAllPaths()
{
    for (int i = 0; i < classPaths.size(); i++) {
        classPaths[i]->clear();
    }
}

void Head::rotateBy (float inputAngle)
{
    angle = inputAngle;
    //draw();
}

void Head::rotate()
{
    for (int i = 0; i < classPaths.size(); ++i)
    {
        classPaths[i]->applyTransform(AffineTransform::rotation(angle, size/ 2.0f, size/ 2.0f));
    }
}

void Head::resized()
{
    size = static_cast<float>( (getLocalBounds().getHeight() > getLocalBounds().getWidth()) ? getLocalBounds().getWidth() : getLocalBounds().getHeight() );
    draw();
}

void Head::translateToOrigin(Path &object)
{
    Rectangle<int> outline = getLocalBounds();
    object.applyTransform(AffineTransform::translation(static_cast<float>(-outline.getX()), static_cast<float>(-outline.getY())));
}

void Head::paint(Graphics& g)
{
    //g.drawImageAt(image, 0, 0);
    g.drawImageTransformed(image, AffineTransform::rotation(angle, image.getWidth()/ 2.0f, image.getHeight()/ 2.0f));
    /*g.setColour (Colours::white);
    for (int i = 0; i < classPaths.size(); i++)
    {
        g.fillPath(*classPaths[i]);
    }*/
}
