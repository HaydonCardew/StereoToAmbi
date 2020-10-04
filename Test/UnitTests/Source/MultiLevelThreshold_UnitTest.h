/*
  ==============================================================================

    MultiLevelThreshold_UnitTest.h
    Created: 29 Sep 2020 9:40:16pm
    Author:  Haydon Cardew

  ==============================================================================
*/

#pragma once
#include "MultiLevelThreshold.h"
#include "gtest/gtest.h"
#include "UnitTestTools.hpp"
#include <cassert>

TEST(MultiLevelThreshold, AzimuthExtraction)
{
    int fftOrder = 12;
    int fftSize = pow(2,fftOrder);
    int nSamples = fftSize/2;
    int sr = 48000;
    
    // construct sine wave
    vector<float> azimuths{ 270 };
    vector<float> freqs{ 1100 };
    assert(azimuths.size() == azimuths.size());
    //assert(azimuths.size() % 2 == 0);
    vector<vector<float>> testSignal = Tools::constructStereoSineWaves(freqs, azimuths, sr, nSamples);
    
    // copy to ComplexFft
    vector<MultiLevelThreshold::ComplexFft> stereoTimeBuffer(2, MultiLevelThreshold::ComplexFft(fftSize, 0));
    for ( unsigned i = 0; i < nSamples; i++ )
    {
        stereoTimeBuffer[0][i] = testSignal[0][i];
        stereoTimeBuffer[1][i] = testSignal[1][i];
    }
    
    // get fft's
    dsp::FFT fft(fftOrder);
    vector<MultiLevelThreshold::ComplexFft> stereoFreqBuffer(2, MultiLevelThreshold::ComplexFft(fftSize, 0));
    fft.perform(stereoTimeBuffer[0].data(), stereoFreqBuffer[0].data(), false);
    fft.perform(stereoTimeBuffer[1].data(), stereoFreqBuffer[1].data(), false);
    
    // perform the magic
    //int nThresholds = static_cast<int>(azimuths.size()/2) - 1;
    int nThresholds = 1;
    int nExtractedSources = 2 * (nThresholds+1);
    int nHistogramBins = 100;
    vector<MultiLevelThreshold::ComplexFft> extractedFfts(nExtractedSources, MultiLevelThreshold::ComplexFft(fftSize, 0));
    MultiLevelThreshold mlt(nThresholds, fftSize, nHistogramBins);
    vector<float> extractedAzimuths(nExtractedSources, 0.f);
    mlt.stereoFftToAmbiFft(stereoFreqBuffer, extractedFfts, extractedAzimuths, 360.f, 10.f, sr);
    for ( int i = 0; i < nExtractedSources; ++i )
    {
        cout << "Azimuth : " << extractedAzimuths[i] << " Power : " << Tools::getAverageMag(extractedFfts[i]) << endl;
    }
    
    std::sort(azimuths.begin(), azimuths.end());
    cout << "Azimuths: " << endl;
    for ( auto &a : azimuths )
    {
        cout << a << endl;
    }
    std::sort(extractedAzimuths.begin(), extractedAzimuths.end());
    cout << "Extracted Azimuths: " << endl;
    for ( auto &a : extractedAzimuths )
    {
        cout << a << endl;
    }
    
    EXPECT_TRUE(true);
}
