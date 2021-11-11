#pragma once
#include "WindowedFIFOBuffer.h"

class MultiChannelWindowedFIFOBuffer
{
public:
    MultiChannelWindowedFIFOBuffer(unsigned nChannels, unsigned windowSize);
    shared_ptr<WindowedFIFOBuffer> getChannel(unsigned channel);
    bool windowedAudioAvailable();
    void write(vector<const float*> readBuffers, unsigned nSamples, float gain = 1.0);
    void read(vector<float*> writeBuffers, unsigned nSamples);
    void getWindowedAudio(vector<vector<float>>& writeBuffers);
    void sendProcessedWindows(vector<vector<float>>& writeBuffers);
    void getWindowedAudio(vector<vector<complex<float>>>& writeBuffers);
    void sendProcessedWindows(vector<vector<complex<float>>>& writeBuffers);
    unsigned outputSamplesAvailable();
    unsigned size();
    void clear();
    
protected:
    vector<shared_ptr<WindowedFIFOBuffer>> buffers;
    //vector<WindowedFIFOBuffer> testBuffers; one day!
    //for debug
    bool sanityCheck();
    unsigned windowSize;
    vector<vector<float>> transferBuffer;
};