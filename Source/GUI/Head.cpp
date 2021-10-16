#include "Head.h"
#include "Assets.h"

Head::Head()
{
    angle = 0;
    image = ImageCache::getFromMemory(Assets::Head_png, Assets::Head_pngSize);
}

void Head::rotateBy (float inputAngle)
{
    angle = inputAngle;
    angle = angle >= (2 * 3.14) ? 0 : angle;
    angle = angle < 0 ? 0 : angle;
}

void Head::paint(Graphics& g)
{
    const float offset = image.getHeight() - image.getWidth();
    g.drawImageTransformed(image, AffineTransform::rotation(angle, image.getWidth()/ 2.0f, image.getHeight()/ 2.0f).followedBy(AffineTransform::translation(offset, 0.f)));
}
