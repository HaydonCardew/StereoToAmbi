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

#include <iostream>
#include <cmath>

Deverb::Deverb (unsigned fftSize) : fftSize(fftSize)
{
    reset();
}

void Deverb::reset()
{
    alpha.clear();
    alpha.resize(fftSize, 0.5);
}

void Deverb::deverberate ( const StereoFftArray& audio, StereoFftArray& direct, StereoFftArray& ambience, float phiZero, float forgetFactor, float mu0, float mu1,  float sigma)
{
    assert(audio.size() == STEREO
           && direct.size() == STEREO
           && ambience.size() == STEREO
           && audio[0].size() == fftSize
           && direct[0].size() == fftSize
           && ambience[0].size() == fftSize);
    for (unsigned i = 0; i < fftSize; ++i)
    {
        float corr = getCorrelation(audio[LEFT][i], audio[RIGHT][i]);
        alpha[i] = forgetFactor*alpha[i] + (1-forgetFactor) * corr;
        const float gamma = scaleCorrelation(alpha[i], mu0, mu1, sigma, phiZero);
        direct[LEFT][i] = audio[LEFT][i] * gamma;
        direct[RIGHT][i] = audio[RIGHT][i] * gamma;
        ambience[LEFT][i] = audio[LEFT][i] * (1 - gamma);
        ambience[RIGHT][i] = audio[RIGHT][i] * (1 - gamma);
    }
}

inline float Deverb::scaleCorrelation(float corr, float mu0, float mu1, float sigma, float phiZero)
{
    const float muAverage = (mu1+mu0) / 2;
    const float muHalfDiff = (mu1-mu0) / 2;
    const float gamma = (muHalfDiff * tanh(sigma * 3.141 * (phiZero - corr)) ) + muAverage;
    assert ( (gamma <= 1.0) && (gamma >= 0.0) );
    // This function flips the correlation value. Flip it back
    return (1 - gamma);
}

inline std::complex<float> Deverb::getPhi ( std::complex<float> first, std::complex<float> second )
{
    return first * conj(second);
}

inline float Deverb::getCorrelation ( std::complex<float> left, std::complex<float> right )
{
    const bool oneIsZero = (left.real() == 0 && left.imag() == 0) || (right.real() == 0 && right.imag() == 0);
    if (oneIsZero)
    {
        return 0.5f;
    }
    std::complex<float> denom = getPhi(left, left) * getPhi(right, right);
    denom = pow(denom, 0.5);
    if (denom.real() == 0)
    {
        return 1.f;
    }
    std::complex<float> num = getPhi(left, right);
    const float corr = num.real() / denom.real();
    assert ( (corr <= 1.1) && (corr >= -1.1) ); // stupid floats
    // change scale from -1 -> 1 to 0 -> 1
    return (corr + 1.f) / 2.f;
}
