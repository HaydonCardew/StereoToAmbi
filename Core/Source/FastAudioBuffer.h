/*
  ==============================================================================

    FastAudioBuffer.h
    Created: 19 Sep 2020 12:28:52am
    Author:  Haydon Cardew

  ==============================================================================
*/
#pragma once

#include <memory>

using namespace std;

class FastAudioBuffer
{
public:
    enum ErrorCode
    {
        Success,
        BufferOverrun,
        BufferUnderrun
    };
    
    FastAudioBuffer(unsigned size);
    
    ErrorCode pop_front();
    ErrorCode pop_back();
    ErrorCode push_front(float sample);
    ErrorCode push_back(float sample);
    
    float& operator[] (int i);
    unsigned capacity();
    void clear();
    bool empty();
    unsigned size();
    bool full();
    
private:
    unique_ptr<float[]> buffer;
    unsigned head;
    unsigned tail;
    unsigned bufferSize;
    unsigned mask;
};
