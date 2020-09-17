#include "MultiLevelThreshold.h"

#include <numeric>
#include <algorithm>
#include <iostream>
#include <math.h>
#include "Tools.hpp"

//#define PI 3.14159265
#define LEFT 0
#define RIGHT 1
#define STEREO 2

using namespace std;

MultiLevelThreshold::MultiLevelThreshold(int noOfThresholds, int fftSize, int histogramSize)
	: leftHistogram(nHistogramBins), rightHistogram(nHistogramBins), noOfThresholds(noOfThresholds), fftSize(fftSize), sourcesPerChannel(noOfThresholds+1), totalNumberOfSources(sourcesPerChannel*STEREO), nHistogramBins(histogramSize)
{
	thresholds.resize(STEREO, vector<float>(noOfThresholds+1, 0)); // +1 for a maximum value

	panMap.resize(STEREO, vector<float>(fftSize,0));
	magnitude.resize(STEREO, vector<float>(fftSize, 0));

	leftSourceMagnitudes.resize(STEREO, vector<float>(sourcesPerChannel, 0));
	rightSourceMagnitudes.resize(STEREO, vector<float>(sourcesPerChannel, 0));

	Pl.resize(nHistogramBins, vector<float>(nHistogramBins, 0));
	Sl.resize(nHistogramBins, vector<float>(nHistogramBins, 0));
	Pr.resize(nHistogramBins, vector<float>(nHistogramBins, 0));
	Sr.resize(nHistogramBins, vector<float>(nHistogramBins, 0));
}

// width 0 -> 360 // offset 0 -> 360 (anti-clockwise)
void MultiLevelThreshold::stereoFftToAmbiFft(const vector<ComplexFft>& stereoFft, vector<ComplexFft>& ambiFfts, vector<float>& azimuths, const float width, const float offset, const unsigned fs)
{
	if (ambiFfts[0].size() < fftSize
        || stereoFft.size() != 2
		|| stereoFft[0].size() < fftSize
		|| ambiFfts.size() < totalNumberOfSources
		|| azimuths.size() < totalNumberOfSources)
    {
		return;
	}
    calcMagnitudeVectors(stereoFft);
	generatePanMap();
	calcHistogram(100, 4000, fs);
	fastMultiLevelthreshold(); // still not fast enough
	extractAudioSources(stereoFft[LEFT], stereoFft[RIGHT], ambiFfts);
    calculateAzimuths(azimuths, width);
    offsetAngles(azimuths, offset);
}

void MultiLevelThreshold::offsetAngles(vector<float>& azimuths, float offset)
{
    int shift = min(360.f, max(offset, 0.f));
    
    for(auto & azimuth : azimuths)
    {
        azimuth -= shift; // ambisonics is anticlockwise...
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

	for (int i = 0; i < fftSize; i++)
    {
		if (panMap[LEFT][i] == panMap[RIGHT][i])
        {
            assert( panMap[LEFT][i] == 0.f );
			ambiFfts[0][i].real(leftFft[i].real() + rightFft[i].real());
            ambiFfts[0][i].imag(leftFft[i].imag() + rightFft[i].imag());
			leftSourceMagnitudes[LEFT][0] += magnitude[LEFT][i];
			leftSourceMagnitudes[RIGHT][0] += magnitude[RIGHT][i];
		}
		else
        {
			for (int j = 0; j < sourcesPerChannel; j++) { // panmap == 0 for both?
				if ((panMap[LEFT][i] != 0) && (panMap[LEFT][i] <= thresholds[LEFT][j]))
                { // right mag is bigger for all of these? - not sure
					ambiFfts[j][i].real(leftFft[i].real() + rightFft[i].real());
					ambiFfts[j][i].imag(leftFft[i].imag() + rightFft[i].imag());
					leftSourceMagnitudes[LEFT][j] += magnitude[LEFT][i];
					leftSourceMagnitudes[RIGHT][j] += magnitude[RIGHT][i];
					break;
				}
				if ((panMap[RIGHT][i] != 0) && (panMap[RIGHT][i] <= thresholds[RIGHT][j]))
                {
					ambiFfts[j + sourcesPerChannel][i].real(leftFft[i].real() + rightFft[i].real());
					ambiFfts[j + sourcesPerChannel][i].imag(leftFft[i].imag() + rightFft[i].imag());
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
and returns the angle between -width/2 & width/2

*/
void MultiLevelThreshold::calculateAzimuths(vector<float>& azimuths, float width)
{
    assert(azimuths.size() == totalNumberOfSources);
    width /= -2; // this inverts the estiamed scaled angle as ambisonics is retarded and goes anti-clockwise
    for (int i = 0; i < sourcesPerChannel; i++)
    {
        azimuths[i] = estimateScaledAngle(leftSourceMagnitudes[LEFT][i], leftSourceMagnitudes[RIGHT][i]) * width;
        azimuths[i + sourcesPerChannel] = estimateScaledAngle(rightSourceMagnitudes[LEFT][i], rightSourceMagnitudes[RIGHT][i]) * width;
    }
}

// returns a scaling of -1 <-> 1
float MultiLevelThreshold::estimateScaledAngle(const float leftMagnitude, const float rightMagnitude)
{
    float totalSourceMagnitude = sqrt( pow(leftMagnitude,2) + pow(rightMagnitude,2) );
    if(totalSourceMagnitude == 0.f)
    {
        return 0.f;
    }
    float angleInRads = asin(rightMagnitude / totalSourceMagnitude); // returns a scale 0 - pi/2
    const float quarterPi = 0.78539816339;
    return (angleInRads/quarterPi) - 1.f;
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

void MultiLevelThreshold::fastMultiLevelthreshold()
{
	// using http://www.iis.sinica.edu.tw/JISE/2001/200109_01.pdf
    
	std::partial_sum(leftHistogram.getFirstProbabilityBin(), leftHistogram.getLastProbabilityBin(), Pl[0].begin());
    assert(Pl[0].back() > 0.99);
    
	std::partial_sum(rightHistogram.getFirstProbabilityBin(), rightHistogram.getLastProbabilityBin(), Pr[0].begin());
    assert(Pr[0].back() > 0.99);
    
    std::partial_sum(leftHistogram.getFirstWeightedProbabilityBin(), leftHistogram.getLastWeightedProbabilityBin(), Sl[0].begin());
	std::partial_sum(rightHistogram.getFirstWeightedProbabilityBin(), rightHistogram.getLastWeightedProbabilityBin(), Sr[0].begin());
    
	for (int u = 1; u < nHistogramBins; u++)
    {
		for (int v = u; v < nHistogramBins; v++)
        {
			Pl[u][v] = Pl[0][v] - Pl[0][u-1]; // P(u,v)= P(1,v) - P(1,u-1)
			Sl[u][v] = Sl[0][v] - Sl[0][u-1];
			Pr[u][v] = Pr[0][v] - Pr[0][u-1]; // P(u,v)= P(1,v) - P(1,u-1)
			Sr[u][v] = Sr[0][v] - Sr[0][u-1];
		}
	}

    //debug
    /*for(int i = 0; i < Pl.size(); ++i)
    {
        cout << "Pl " << i << " : " << Pl[i][0] << endl;
    }*/
    //debug
    
	// Make 'P' the 'G' to save making a whole new matrix in mem
	// must be faster recursive methods...

	for (int u = 0; u < nHistogramBins; u++)
    {
		for (int v = u; v < nHistogramBins; v++)
        {
            if (Pl[u][v] != 0)
            { //never had this issue in matlab?!
                Pl[u][v] = (Sl[u][v] * Sl[u][v]) / Pl[u][v];
            }
			if (Pr[u][v] != 0)
            { //never had this issue in matlab?!
				Pr[u][v] = (Sr[u][v] * Sr[u][v]) / Pr[u][v];
			}
		}
	}

    Tools::zeroVector(thresholds);
    
	unsigned int rVarSq = 0;
	unsigned int lVarSq = 0;
	unsigned int tmpR = 0;
	unsigned int tmpL = 0;
	switch (noOfThresholds) {
	case 1:
		for (int t1 = 0; t1 < (nHistogramBins - 1); t1++) {
			tmpL = Pl[0][t1] + Pl[t1 + 1][nHistogramBins - 1];
			if (tmpL > lVarSq) {
				lVarSq = tmpL;
				thresholds[LEFT][0] = (float)t1;
			}
			tmpR = Pr[0][t1] + Pr[t1 + 1][nHistogramBins - 1];
			if (tmpR > rVarSq) {
				rVarSq = tmpR;
				thresholds[RIGHT][0] = (float)t1;
			}
		}
		break;
	case 2:
		for (int t1 = 0; t1 < (nHistogramBins - 1); t1++) {
			for (int t2 = (t1 + 1); t2 < (nHistogramBins - 2); t2++) {
				tmpL = Pl[0][t1] + Pl[t1+1][t2] + Pl[t2+1][nHistogramBins-1];
				if (tmpL > lVarSq) {
					lVarSq = tmpL;
					thresholds[LEFT][0] = (float)t1;
					thresholds[LEFT][1] = (float)t2;
				}
				tmpR = Pr[0][t1] + Pr[t1 + 1][t2] + Pr[t2 + 1][nHistogramBins - 1];
				if (tmpR > rVarSq) {
					rVarSq = tmpR;
					thresholds[RIGHT][0] = (float)t1;
					thresholds[RIGHT][1] = (float)t2;
				}
			}
		}
		break;
	case 3:
		for (int t1 = 0; t1 < (nHistogramBins - 1); t1++) {
			for (int t2 = (t1 + 1); t2 < (nHistogramBins - 2); t2++) {
				for (int t3 = (t2 + 1); t3 < (nHistogramBins - 3); t3++) {
					tmpL = Pl[0][t1] + Pl[t1+1][t2] + Pl[t2+1][t3] + Pl[t3+1][nHistogramBins-1];
					if (tmpL > lVarSq) {
						lVarSq = tmpL;
						thresholds[LEFT][0] = (float)t1;
						thresholds[LEFT][1] = (float)t2;
						thresholds[LEFT][2] = (float)t3;
					}
					tmpR = Pr[0][t1] + Pr[t1 + 1][t2] + Pr[t2 + 1][t3] + Pr[t3 + 1][nHistogramBins - 1];
					if (tmpR > rVarSq) {
						rVarSq = tmpR;
						thresholds[RIGHT][0] = (float)t1;
						thresholds[RIGHT][1] = (float)t2;
						thresholds[RIGHT][2] = (float)t3;
					}
				}
			}
		}
		break;
	}
	for (int i = 0; i < noOfThresholds; i++) {
        // for linear
		thresholds[LEFT][i] *= leftHistogram.getIncrement();
		thresholds[RIGHT][i] *= rightHistogram.getIncrement();
        
        // for non-linear
        // only need non-linear for bss technique?
        /*
        thresholds[LEFT][i] = histogram.maxValue[LEFT] * pow((thresholds[LEFT][i] * histogram.increment[LEFT] / histogram.maxValue[LEFT] ), 1); // 0.5 = sqrt()
        thresholds[RIGHT][i] = histogram.maxValue[RIGHT] * pow((thresholds[RIGHT][i] * histogram.increment[RIGHT] / histogram.maxValue[RIGHT] ), 1); // 0.5 = sqrt()
         */
	}
	thresholds[LEFT][noOfThresholds] = leftHistogram.getMaxValue(); //just add source num? +1?
	thresholds[RIGHT][noOfThresholds] = rightHistogram.getMaxValue();
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

void MultiLevelThreshold::calcHistogram(float minFreq, float maxFreq, int fs)
{
    assert( (minFreq > 0) && (minFreq < maxFreq) );
    assert( (maxFreq <= (fs/2)) );
    float freqStep = float(fftSize) / float(fs);
    int kMin = freqStep * minFreq;
    int kMax = freqStep * maxFreq;
    leftHistogram.loadData(panMap[LEFT], kMin, kMax);
    rightHistogram.loadData(panMap[RIGHT], kMin, kMax);
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
