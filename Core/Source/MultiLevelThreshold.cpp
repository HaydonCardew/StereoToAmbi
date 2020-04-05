#include "MultiLevelThreshold.h"

#include <numeric>
#include <algorithm>
#include <iostream>
#include <math.h>

//#define PI 3.14159265
#define LEFT 0
#define RIGHT 1
#define STEREO 2

using namespace std;

MultiLevelThreshold::MultiLevelThreshold(int noOfThresholds, int fftSize, int histogramSize, int ambiOrder)
	: leftHistogram(nHistogramBins), rightHistogram(nHistogramBins), ambiOrder(ambiOrder), noOfThresholds(noOfThresholds), fftSize(fftSize), sourcesPerChannel(noOfThresholds+1), totalNumberOfSources(sourcesPerChannel*STEREO), nHistogramBins(histogramSize)
{
	thresholds.resize(STEREO, vector<float>(noOfThresholds+1, 0)); // +1 for a maximum value

	panMap.resize(STEREO, vector<float>(fftSize,0));
	magnitude.resize(STEREO, vector<float>(fftSize, 0));

	leftSourceMagnitudes.resize(STEREO, vector<float>(fftSize, 0));
	rightSourceMagnitudes.resize(STEREO, vector<float>(fftSize, 0));

	Pl.resize(nHistogramBins, vector<float>(nHistogramBins, 0));
	Sl.resize(nHistogramBins, vector<float>(nHistogramBins, 0));
	Pr.resize(nHistogramBins, vector<float>(nHistogramBins, 0));
	Sr.resize(nHistogramBins, vector<float>(nHistogramBins, 0));
	azimuth.resize(STEREO, vector<float>(sourcesPerChannel, 0));
}

inline void MultiLevelThreshold::zeroVector(vector<vector<float>> &input)
{
    for(auto & i : input)
    {
        fill(i.begin(), i.end(), 0);
    }
}

// width 0 -> 360
void MultiLevelThreshold::stereoFftToAmbiFft(const ComplexFft& leftFft, const ComplexFft& rightFft, vector<ComplexFft>& ambiFfts, vector<float>& azimuths, const int width, const int offset)
{
	if (ambiFfts[0].size() < fftSize
		|| leftFft.size() < fftSize
		|| rightFft.size() < fftSize
		|| ambiFfts.size() < totalNumberOfSources
		|| azimuths.size() < totalNumberOfSources) {
		return;
	}
    calcMagnitudeVectors(leftFft, rightFft);
	generatePanMap();
    int fs = 48000;
	calcHistogram(100, 4000, fs);
	fastMultiLevelthreshold(); // still not fast enough
	extractAudioSources(leftFft, rightFft, ambiFfts, azimuths, width);
    offsetAngles(azimuths, offset);
}

// Shift where centre is. Input -180 -> 180.
void MultiLevelThreshold::offsetAngles(vector<float>& azimuths, int offset)
{
    offset = min(180, offset);
    offset = max(-180, offset);
    for(auto & azimuth : azimuths)
    {
        azimuth += offset;
    }
}

void MultiLevelThreshold::extractAudioSources(const ComplexFft& leftFft, const ComplexFft& rightFft, vector<ComplexFft>& ambiFfts, vector<float>& azimuths, int width)
{
	//set all to zero? (sources - azimuth)
	//set thresholds to have an extra 'threshold' which is the max
    zeroVector(leftSourceMagnitudes);
    zeroVector(rightSourceMagnitudes);

	for (int i = 0; i < totalNumberOfSources; i++)
    {
		std::fill(ambiFfts[i].begin(), ambiFfts[i].end(), 0);
	}

    //vector<vector<int>> nonZeroBinsInSource(STEREO, vector<int>(sourcesPerChannel, 0));

	for (int i = 0; i < fftSize; i++)
    {
		if (panMap[LEFT][i] == 0 && panMap[RIGHT][i] == 0)
        {
			ambiFfts[0][i] = leftFft[i].real() + rightFft[i].real();
			ambiFfts[0][i] = leftFft[i].imag() + rightFft[i].imag();
			leftSourceMagnitudes[LEFT][0] += magnitude[LEFT][i];
			leftSourceMagnitudes[RIGHT][0] += magnitude[RIGHT][i];
            //nonZeroBinsInSource[LEFT][0]++;
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
                    //nonZeroBinsInSource[LEFT][j]++;
					break;
				}
				if ((panMap[RIGHT][i] != 0) && (panMap[RIGHT][i] <= thresholds[RIGHT][j]))
                {
					ambiFfts[j + sourcesPerChannel][i].real(leftFft[i].real() + rightFft[i].real());
					ambiFfts[j + sourcesPerChannel][i].imag(leftFft[i].imag() + rightFft[i].imag());
					rightSourceMagnitudes[LEFT][j] += magnitude[LEFT][i];
					rightSourceMagnitudes[RIGHT][j] += magnitude[RIGHT][i];
                    //nonZeroBinsInSource[RIGHT][j]++;
					break;
				}
			}
		}
	}
    
    zeroVector(azimuth);
    
    width /= 2; // right is positive and left is negative so this works
	for (int i = 0; i < sourcesPerChannel; i++)
    {
        azimuth[LEFT][i] = estimateScaledAngle(leftSourceMagnitudes[LEFT][i], leftSourceMagnitudes[RIGHT][i]);
        azimuth[LEFT][i] *= width;
        azimuth[RIGHT][i] = estimateScaledAngle(rightSourceMagnitudes[LEFT][i], rightSourceMagnitudes[RIGHT][i]);
        azimuth[RIGHT][i] *= width;
	}

	for (int i = 0; i < sourcesPerChannel; i++)
    {
		azimuths[i] = azimuth[LEFT][i];
		azimuths[i + sourcesPerChannel] = azimuth[RIGHT][i];
	}
}

// returns a scaling of -1 <-> 1
float MultiLevelThreshold::estimateScaledAngle(float leftMagnitude, float rightMagnitude)
{
    float totalSourceMagnitude = sqrt( pow(leftMagnitude,2) + pow(rightMagnitude,2) );
    if(totalSourceMagnitude == 0.f)
    {
        return 0.f;
    }
    float angleInRads = asin(rightMagnitude / totalSourceMagnitude); // returns a scale 0 - pi/2
    const float quarterPi = 0.78539816339;
    //cout << "Left Mag: " << leftMagnitude << " Right Mag: " << rightMagnitude << " Angle : " << (angleInRads/quarterPi) - 1 << endl;
    return (angleInRads/quarterPi) - 1.f;
}

void MultiLevelThreshold::calcMagnitudeVectors(const ComplexFft& leftFft, const ComplexFft& rightFft)
{
	for (int i = 0; i < magnitude[LEFT].size(); i++)
    {
        magnitude[LEFT][i] = abs(leftFft[i]);
        magnitude[RIGHT][i] = abs(rightFft[i]);
	}
}

void MultiLevelThreshold::fastMultiLevelthreshold()
{
	// using http://www.iis.sinica.edu.tw/JISE/2001/200109_01.pdf
    
	std::partial_sum(leftHistogram.getFirstProbabilityBin(), leftHistogram.getLastProbabilityBin(), Pl[0].begin());
	std::partial_sum(rightHistogram.getFirstProbabilityBin(), rightHistogram.getLastProbabilityBin(), Pr[0].begin());

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

    zeroVector(thresholds);
    
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
    zeroVector(panMap);
    for (int i = 0; i < magnitude[LEFT].size(); i++)
    {
        if (magnitude[RIGHT][i] == 0.0)
        {
			magnitude[RIGHT][i] = 1e-7;
		} //small enough?
        double tmp = 20*log(magnitude[LEFT][i] / magnitude[RIGHT][i]);

        if (tmp >= 0)
        {
			panMap[LEFT][i] = tmp;
        }
        else
        {
			panMap[RIGHT][i] = -1 * tmp;
        }
    }
}

void MultiLevelThreshold::calcHistogram(float minFreq, float maxFreq, int fs)
{
    float freqStep = float(nHistogramBins) / float(fs);
    int kMin = freqStep * minFreq;
    int kMax = freqStep * maxFreq;
    
    leftHistogram.loadData(panMap[LEFT], kMin, kMax);
    rightHistogram.loadData(panMap[RIGHT], kMin, kMax);
}

void MultiLevelThreshold::testMultiLevelThreshold(float* data, int nDataPoints, float* thresholds, int nThresholds, int nBins)
{
    
    bool debug = false;
    
    // Resize Pl Sl
    nHistogramBins = nBins; // hist size
    vector<vector<float>> P(nHistogramBins, vector<float>(nHistogramBins, 0));
    vector<vector<float>> S(nHistogramBins, vector<float>(nHistogramBins, 0));
    vector<vector<float>> G(nHistogramBins, vector<float>(nHistogramBins, 0));
    
    vector<float> input(nDataPoints);
    
    for(int i = 0; i < nDataPoints; ++i)
    {
        input[i] = *data;
        data++;
    }
    
    // Create histo
    auto max = *max_element(input.begin(), input.end());
    
    float histIncre = max/nHistogramBins;
    vector<int> hist(nHistogramBins, 0);
    
    for(int i = 0; i < nDataPoints; ++i)
    {
        int index = floor(input[i] / histIncre);
        if(index >= hist.size()) { index = (int)(hist.size() - 1); }
        hist[index]++;
    }
    
    if(debug)
    {
        cout.precision(2);
        cout << "Hist: " << endl;
        for(int i = 0; i < hist.size(); ++i)
        {
            cout << hist[i] << " ";
        }
        cout << endl;
    }
    
    // Histo into probability histo
    
    vector<float> histP(nHistogramBins, 0);
    float accu = accumulate(hist.begin(), hist.end(), 0);
    for(int i = 0; i < hist.size(); ++i)
    {
        histP[i] = (float)hist[i]/accu;
    }
    
    if(debug)
    {
        cout << "Hist (probs): " << endl;
        for(int i = 0; i < histP.size(); ++i)
        {
            cout << histP[i] << " ";
        }
        cout << endl;
    }
    
    std::partial_sum(histP.begin(), histP.end(), P[0].begin());

      // fast way to do recursion below?

      for (int i = 0; i < nHistogramBins; i++) {
          //histogram.bins[LEFT][i] = histogram.bins[LEFT][i] * (i+1);
          histP[i] = histP[i] * (i+1);
      }

    std::partial_sum(histP.begin(), histP.end(), S[0].begin());

      for (int u = 1; u < nHistogramBins; u++) {
          for (int v = u; v < nHistogramBins; v++) {
              P[u][v] = P[0][v] - P[0][u-1]; // P(u,v)= P(1,v) - P(1,u-1) in matlab
              S[u][v] = S[0][v] - S[0][u-1];
          }
      }

      // Make 'P' the 'G' to save making a whole new matrix in mem
      // must be faster recursive methods...

      for (int u = 0; u < nHistogramBins; u++) {
          for (int v = u; v < nHistogramBins; v++) {
            if (P[u][v] != 0) { //never had this issue in matlab?!
              G[u][v] = (S[u][v] * S[u][v]) / P[u][v];
            }
            else {
                if(true)
                {
                cout << "Its happened." << endl;
                }
            }
          }
      }

    if(debug)
    {
        cout << "P: " << endl;
        for(int u = 0; u < nHistogramBins; ++u)
        {
            for(int v = 0; v < nHistogramBins; ++v)
            {
                cout << P[u][v] << " ";
            }
            cout << endl;
        }
        
        cout << "S: " << endl;
        for(int u = 0; u < nHistogramBins; ++u)
        {
            for(int v = 0; v < nHistogramBins; ++v)
            {
                cout << S[u][v] << " ";
            }
            cout << endl;
        }
        
        cout << "G: " << endl;
        for(int u = 0; u < nHistogramBins; ++u)
        {
            for(int v = 0; v < nHistogramBins; ++v)
            {
                cout << G[u][v] << " ";
            }
            cout << endl;
        }
    }
    
      float lVarSq = 0;
      float tmpL = 0;
      switch (nThresholds) {
      case 1:
          for (int t1 = 0; t1 < (nHistogramBins - 1); t1++) {
              tmpL = G[0][t1] + G[t1+1][nHistogramBins-1];
              if (tmpL > lVarSq) {
                  lVarSq = tmpL;
                  thresholds[0] = t1;
              }
          }
          break;
      case 2:
          for (int t1 = 0; t1 < (nHistogramBins - 1); t1++) {
              for (int t2 = (t1 + 1); t2 < (nHistogramBins - 2); t2++) {
                  tmpL = G[0][t1] + G[t1+1][t2] + G[t2+1][nHistogramBins-1];
                  if (tmpL > lVarSq) {
                      lVarSq = tmpL;
                      thresholds[0] = t1;
                      thresholds[1] = t2;
                  }
              }
          }
          break;
      case 3:
          for (int t1 = 0; t1 < (nHistogramBins - 1); t1++) {
              for (int t2 = (t1 + 1); t2 < (nHistogramBins - 2); t2++) {
                  for (int t3 = (t2 + 1); t3 < (nHistogramBins - 3); t3++) {
                      tmpL = G[0][t1] + G[t1+1][t2] + G[t2+1][t3] + G[t3+1][nHistogramBins-1];
                      if (tmpL > lVarSq) {
                          lVarSq = tmpL;
                          thresholds[0] = t1;
                          thresholds[1] = t2;
                          thresholds[2] = t3;
                      }
                  }
              }
          }
          break;
      }
      for (int i = 0; i < nThresholds; i++) {
          thresholds[i] *= histIncre;
      }
      //thresholds[LEFT][noOfThresholds] = histogram.maxValue[LEFT]; //just add source num? +1?
     
}

void MultiLevelThreshold::getLastHisto(vector<int>& leftBin, vector<float>& leftProb)
{
    //leftBin = histogram.bins[LEFT];
    //leftProb = histogram.probabilityBins[LEFT];
}

extern "C" {
    MultiLevelThreshold* MT_new(int noOfThresholds, int fftSize, int histogramSize, int ambiOrder){ return new MultiLevelThreshold(noOfThresholds, fftSize, histogramSize, ambiOrder); }
    int MT_getNumberOfExtractedSources(MultiLevelThreshold* MT) { return MT->getNumberOfExtractedSources(); }
void MT_multiLevelThreshold(MultiLevelThreshold* MT, float *data, int nDataPoints, float *thresholds, int nThresholds, int nBins) { return MT->testMultiLevelThreshold(data, nDataPoints, thresholds, nThresholds, nBins); }
void MT_delete(MultiLevelThreshold* MT) { delete MT; }
}
