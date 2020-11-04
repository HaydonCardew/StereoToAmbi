/*
  ==============================================================================

    Deverb.h
    Created: 1 Nov 2020 1:31:51am
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include <vector>
#include <complex>

class Deverb
{
// https://www.computer.org/csdl/pds/api/csdl/proceedings/download-article/12OmNvzJG87/pdf
public:
    typedef std::vector<std::complex<float>> FftArray;
    typedef std::vector<FftArray> StereoFftArray;
    Deverb (unsigned fftSize);
    void reset();
    void deverberate ( const StereoFftArray& audio, StereoFftArray& direct, StereoFftArray& ambience );
private:
    const unsigned fftSize;
    std::vector<float> alpha;
    inline std::complex<float> getPhi ( std::complex<float> first, std::complex<float> second );
    inline float getCorrelation ( std::complex<float> left, std::complex<float> right );
    enum
    {
        LEFT = 0,
        RIGHT = 1,
        STEREO = 2
    };
    const float forgetFactor = 0.85;
    
    float scaleCorrelation ( const float corr );
    const float phiZero = 0.15;
    const float mu0 = 0.0; // min
    const float mu1 = 1; // max
    const float sigma = 4; // curve
};
