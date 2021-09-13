/*
  ==============================================================================

    Head.h
    Created: 10 Nov 2020 10:35:28pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class Head : public Component
{
    public:
    Head ();

    Image image;
    
    Path head;
    Path rightEar;
    Path leftEar;
    Path nose;
    std::vector<Path*> classPaths;

    float angle;
    float size;

    void draw();
    void clearAllPaths();

    void rotateBy(float inputAngle);

    void rotate();

    void resized() override;

    void translateToOrigin(Path &object);
    void paint(Graphics& g) override;
};
