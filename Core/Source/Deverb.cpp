/*
  ==============================================================================

    Deverb.cpp
    Created: 1 Nov 2020 1:31:51am
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "Deverb.h"
#include "Tools.h"
#include <math.h>

Deverb::Deverb (unsigned fftSize) : fftSize(fftSize)
{
    reset();
}

void Deverb::reset()
{
    alpha.clear();
    alpha.resize(fftSize, 0.5);
}

void Deverb::deverberate ( const StereoFftArray& audio, StereoFftArray& direct, StereoFftArray& ambience )
{
    assert(audio.size() == STEREO
           && direct.size() == STEREO
           && ambience.size() == STEREO
           && audio[0].size() == fftSize
           && direct[0].size() == fftSize
           && ambience[0].size() == fftSize);
    for (unsigned i = 0; i < fftSize; ++i)
    {
        const float corr = getCorrelation(audio[LEFT][i], audio[RIGHT][i]);
        alpha[i] = forgetFactor*alpha[i] + (1-forgetFactor)*corr;
        const float gamma = getGamma(corr);
        direct[LEFT][i] = audio[LEFT][i] * gamma;
        direct[RIGHT][i] = audio[RIGHT][i] * gamma;
        ambience[LEFT][i] = audio[LEFT][i] * (1 - gamma);
        ambience[RIGHT][i] = audio[RIGHT][i] * (1 - gamma);
    }
}

inline float Deverb::getGamma(float corr)
{
    const float muAverage = (mu1-mu0) / 2;
    return (muAverage * tanh(sigma * 3.141 * (phiZero - corr)) ) + muAverage;
}

inline std::complex<float> Deverb::getPhi ( std::complex<float> first, std::complex<float> second )
{
    return first * conj(second);
}

inline float Deverb::getCorrelation ( std::complex<float> left, std::complex<float> right )
{
    std::complex<float> denom = getPhi(left, left) * getPhi(right, right);
    denom = pow(denom, 0.5);
    std::complex<float> num = getPhi(left, right);
    return num.real() / denom.real();
}
