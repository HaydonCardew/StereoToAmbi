/*
  ==============================================================================

    TestWindowedFIFOBuffer.cpp
    Created: 13 Sep 2020 1:26:07am
    Author:  Haydon Cardew

  ==============================================================================
*/

#include "WindowedFIFOBuffer.h"

extern "C" {
    WindowedFIFOBuffer* WFIFOB_new(int windowSize, float overlap) { return new WindowedFIFOBuffer(windowSize, overlap); }
    unsigned WFIFOB_write(WindowedFIFOBuffer* WB, float *data, unsigned nSamples, float gain) { return WB->write(data, nSamples, gain); }
    unsigned WFIFOB_read(WindowedFIFOBuffer* WB, float *data, unsigned nSamples) { return WB->read(data, nSamples); }
    void WFIFOB_sendProcessedWindow(WindowedFIFOBuffer* WB, float* data) { return WB->sendProcessedWindow(data); }
    int WFIFOB_getWindowedAudio(WindowedFIFOBuffer* WB, float* data) { return WB->getWindowedAudio(data); }
    void WFIFOB_delete(WindowedFIFOBuffer* WB) { delete WB; }
}

// data assumed to be size of window size!
int WindowedFIFOBuffer::getWindowedAudio(float *data)
{
    if (!windowedAudioAvailable())
    {
        return 0;
    }
    vector<float> buffer(windowSize);
    if(getWindowedAudio(buffer))
    {
        for (int i = 0; i < windowSize; ++i)
        {
            data[i] = buffer[i];
        }
        return windowSize;
    }
    else
    {
        return -1;
    }
}

// data assumed to be size of window size!
void WindowedFIFOBuffer::sendProcessedWindow(float *data)
{
    vector<float> buffer(windowSize);
    for (int i = 0; i < windowSize; ++i)
    {
        buffer[i] = data[i];
    }
    sendProcessedWindow(buffer);
}
