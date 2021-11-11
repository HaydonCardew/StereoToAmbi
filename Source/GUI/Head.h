#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class Head : public Component
{
public:
    Head ();
    void rotateBy(float inputAngle);
    void paint(Graphics& g) override;
    
private:
    Image image;
    float angle;
};
