/*
  ==============================================================================

    AzimuthView.cpp
    Created: 10 Nov 2020 10:36:37pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "AzimuthView.h"

AzimuthView::AzimuthView() : rads(0), offset(0)
{
    drawPie();
}

void AzimuthView::changeAzimuth(float azi)
{
    azi /= 2;
    if (rads != azi && 6.282 >= azi && azi >= 0.f)
    {
        rads = azi;
        drawPie();
    }
}

void AzimuthView::changeOffset(float azi)
{
    offset = azi;
    offset = offset >= (2 * 3.14) ? 0 : offset;
    offset = offset < 0 ? 0 : offset;
    drawPie();
}

void AzimuthView::paint(Graphics& g)
{
    g.setColour(juce::Colour (0, 255, 255).withAlpha(0.5f));
    g.fillPath(segment);
}

void AzimuthView::resized()
{
    drawPie();
}

void AzimuthView::drawPie()
{
    segment.clear();
    int size = getSquareSize();
    segment.addPieSegment(0.0f, 0.0f, size, size, -rads+offset, rads+offset, 0.0f);
    repaint();
}

int AzimuthView::getSquareSize()
{
    return getWidth() > getHeight() ? getHeight(): getWidth();
}
