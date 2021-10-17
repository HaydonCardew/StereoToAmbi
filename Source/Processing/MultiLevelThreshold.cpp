#include "MultiLevelThreshold.h"
#include <numeric>
#include <algorithm>
#include <iostream>
#include <math.h>
#include "Tools.h"

#define LEFT 0
#define RIGHT 1
#define STEREO 2

using namespace std;

MultiLevelThreshold::MultiLevelThreshold(int noOfThresholds, int fftSize, int histogramSize)
	: fftSize(fftSize), nHistogramBins(histogramSize), leftHistogram(nHistogramBins), rightHistogram(nHistogramBins), noOfThresholds(noOfThresholds), sourcesPerChannel(noOfThresholds+1), totalNumberOfSources(sourcesPerChannel*STEREO)
{
	panMap.resize(STEREO, vector<float>(fftSize,0));
	magnitude.resize(STEREO, vector<float>(fftSize, 0));

	leftSourceMagnitudes.resize(STEREO, vector<float>(sourcesPerChannel, 0));
	rightSourceMagnitudes.resize(STEREO, vector<float>(sourcesPerChannel, 0));

    histogram.resize(STEREO, Histogram(histogramSize, noOfThresholds));
    
    thresholds.resize(STEREO, vector<float>(noOfThresholds+1, 0)); // +1 for a maximum value
}

// width 0 -> 360 // offset 0 -> 360 (anti-clockwise)
void MultiLevelThreshold::stereoFftToAmbiFft(vector<ComplexFft>& stereoFft, vector<ComplexFft>& ambiFfts, vector<float>& azimuths, const float width, const float offset, const unsigned fs)
{
	if (ambiFfts[0].size() < fftSize
        || stereoFft.size() != 2
		|| stereoFft[0].size() < fftSize
		|| ambiFfts.size() < totalNumberOfSources
		|| azimuths.size() < totalNumberOfSources)
    {
		return;
	}
    for ( int i = (fftSize/2); i < fftSize; ++i )
    {
        stereoFft[LEFT][i] = 0;
        stereoFft[RIGHT][i] = 0;
    }
    calcMagnitudeVectors(stereoFft);
	generatePanMap();
	loadHistograms(100, 4000, fs);
	extractAudioSources(stereoFft[LEFT], stereoFft[RIGHT], ambiFfts);
    calculateAzimuths(azimuths, width);
    offsetAngles(azimuths, offset);
}

void MultiLevelThreshold::offsetAngles(vector<float>& azimuths, float offset)
{
    int shift = min(360.f, max(offset, 0.f));
    
    for ( auto & azimuth : azimuths )
    {
        azimuth -= shift; //  take off as ambisonics goes anti-clockwise
        if (azimuth > 360.f)
        {
            azimuth -= 360.f;
        }
        if (azimuth < 0.f)
        {
            azimuth += 360.f;
        }
    }
}

void MultiLevelThreshold::extractAudioSources(const ComplexFft& leftFft, const ComplexFft& rightFft, vector<ComplexFft>& ambiFfts)
{
	//set thresholds to have an extra 'threshold' which is the max
    Tools::zeroVector(leftSourceMagnitudes);
    Tools::zeroVector(rightSourceMagnitudes);
    Tools::zeroVector(ambiFfts);
    
    thresholds[LEFT] = histogram[LEFT].getThresholdValues();
    thresholds[LEFT].push_back(histogram[LEFT].getMaxValue());
    thresholds[RIGHT] = histogram[RIGHT].getThresholdValues();
    thresholds[RIGHT].push_back(histogram[RIGHT].getMaxValue());
    
	for (int i = 0; i < fftSize; i++)
    {
		if (panMap[LEFT][i] == panMap[RIGHT][i])
        {
            assert( panMap[LEFT][i] == 0.f );
            ambiFfts[0][i] = leftFft[i] + rightFft[i];
			leftSourceMagnitudes[LEFT][0] += magnitude[LEFT][i];
			leftSourceMagnitudes[RIGHT][0] += magnitude[RIGHT][i];
		}
		else
        {
			for (int j = 0; j < sourcesPerChannel; j++)
            { // panmap == 0 for both?
				if ((panMap[LEFT][i] != 0) && (panMap[LEFT][i] <= thresholds[LEFT][j]))
                { // right mag is bigger for all of these? - not sure
                    ambiFfts[j][i] = leftFft[i] + rightFft[i];
					leftSourceMagnitudes[LEFT][j] += magnitude[LEFT][i];
					leftSourceMagnitudes[RIGHT][j] += magnitude[RIGHT][i];
					break;
				}
				if ((panMap[RIGHT][i] != 0) && (panMap[RIGHT][i] <= thresholds[RIGHT][j]))
                {
                    ambiFfts[j + sourcesPerChannel][i] = leftFft[i] + rightFft[i];
					rightSourceMagnitudes[LEFT][j] += magnitude[LEFT][i];
					rightSourceMagnitudes[RIGHT][j] += magnitude[RIGHT][i];
					break;
				}
			}
		}
	}
}

/*
This needs to take in width in degrees, get a scaled angle between -1 & 1
and returns the angle between 0 <-> 360
*/
void MultiLevelThreshold::calculateAzimuths(vector<float>& azimuths, float width)
{
    assert(azimuths.size() == totalNumberOfSources);
    width = -(width/2); // this inverts the estimated scaled angle as ambisonics is retarded and goes anti-clockwise
    for (int i = 0; i < sourcesPerChannel; i++)
    {
        azimuths[i] = estimateScaledAngle(leftSourceMagnitudes[LEFT][i], leftSourceMagnitudes[RIGHT][i]) * width;
        azimuths[i + sourcesPerChannel] = estimateScaledAngle(rightSourceMagnitudes[LEFT][i], rightSourceMagnitudes[RIGHT][i]) * width;
    }
}

// returns a scaling between -1 <-> 1 (L <-> R)
float MultiLevelThreshold::estimateScaledAngle(const float leftMagnitude, const float rightMagnitude)
{
    if (leftMagnitude == 0.f)
    {
        return 0.f;
    }
    float angleInRads = atan(rightMagnitude / leftMagnitude); // returns a scale 0 - pi/2
    const float quarterPi = 3.14159265 / 4;
    return (angleInRads/quarterPi) - 1.0f;
}

void MultiLevelThreshold::calcMagnitudeVectors(const vector<ComplexFft>& stereoFft)
{
    assert(stereoFft[LEFT].size() == magnitude[LEFT].size());
    for (unsigned channel = 0; channel < STEREO; ++channel)
    {
        for (int i = 0; i < magnitude[0].size(); i++)
        {
            magnitude[channel][i] = abs(stereoFft[channel][i]);
            assert(magnitude[channel][i] >= 0.f);
        }
    }
}

void MultiLevelThreshold::generatePanMap()
{
    Tools::zeroVector(panMap);
    for (int i = 0; i < magnitude[LEFT].size(); i++)
    {
        float rightMag = magnitude[RIGHT][i];
        if (rightMag == 0.f)
        {
            rightMag = 1e-7; // do this to not change the actual magnitude vector!
		}
        float powerDiff = 0;
        if(magnitude[LEFT][i] != 0.f) // if it is? why can't it be zero?
        {
            powerDiff = 20*log(magnitude[LEFT][i] / rightMag);
        }
        //assert(!isinf(tmp));
        //assert(!isnan(tmp));
        if (powerDiff >= 0)
        {
			panMap[LEFT][i] = powerDiff;
        }
        else
        {
			panMap[RIGHT][i] = -1 * powerDiff;
        }
    }
}

void MultiLevelThreshold::loadHistograms(float minFreq, float maxFreq, int fs)
{
    assert( (minFreq > 0) && (minFreq < maxFreq) );
    assert( (maxFreq <= (fs/2)) );
    float freqStep = float(fftSize) / float(fs);
    int kMin = freqStep * minFreq;
    int kMax = freqStep * maxFreq;
    histogram[LEFT].loadData(panMap[LEFT], kMin, kMax);
    histogram[RIGHT].loadData(panMap[RIGHT], kMin, kMax);
}

vector<float> MultiLevelThreshold::getTotalSourceMagnitudes()
{
    vector<float> totalSourceMagnitudes(totalNumberOfSources + 1, 0);
    for (int i = 0; i < sourcesPerChannel; i++)
    {
        totalSourceMagnitudes[i] = leftSourceMagnitudes[LEFT][i] + leftSourceMagnitudes[RIGHT][i];
        totalSourceMagnitudes[i + sourcesPerChannel] = rightSourceMagnitudes[LEFT][i] + rightSourceMagnitudes[RIGHT][i];
    }
    totalSourceMagnitudes[totalNumberOfSources + 1] = *std::max_element(totalSourceMagnitudes.begin(), totalSourceMagnitudes.end());
    for(int i = 0; i < totalSourceMagnitudes.size(); ++i)
    {
        totalSourceMagnitudes[i] /= totalSourceMagnitudes[totalNumberOfSources + 1];
    }
    return totalSourceMagnitudes;
}
