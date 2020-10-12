/*
==============================================================================

FastAudioBuffer.cpp
Created: 19 Sep 2020 12:28:52am
Author:  Haydon Cardew

==============================================================================
*/

#include "FastAudioBuffer.h"
#include <iostream>

FastAudioBuffer::FastAudioBuffer(unsigned size) : head(0), tail(0)
{
    bufferSize = 1;
    while (bufferSize < size)
    {
        bufferSize <<= 1;
    }
    buffer = make_unique<float[]>(bufferSize);
    mask = bufferSize - 1;
}

float& FastAudioBuffer::operator[] (int i)
{
    if ( i >= size())
    {
        cerr << "Access out of bounds" << endl;
        throw runtime_error("Access out of bounds"); // is this correct?
    }
    return buffer[(head + i) & mask];
}

unsigned FastAudioBuffer::capacity()
{
    return bufferSize;
}

FastAudioBuffer::ErrorCode FastAudioBuffer::pop_front()
{
    if ( empty() )
    {
        return BufferUnderrun;
    }
    ++head;
    head &= mask;
    return Success;
}

FastAudioBuffer::ErrorCode FastAudioBuffer::pop_back()
{
    if ( empty() )
    {
        return BufferUnderrun;
    }
    --tail;
    tail &= mask;
    return Success;
}

FastAudioBuffer::ErrorCode FastAudioBuffer::push_back(float sample)
{
    if ( full() )
    {
        return BufferOverrun;
    }
    buffer[tail] = sample;
    ++tail;
    tail &= mask;
    return Success;
}

FastAudioBuffer::ErrorCode FastAudioBuffer::push_front(float sample)
{
    if ( full() )
    {
        return BufferOverrun;
    }
    --head;
    head &= mask;
    buffer[head] = sample;
    return Success;
}

void FastAudioBuffer::clear()
{
    head = tail;
}

bool FastAudioBuffer::empty()
{
    return head == tail;
}

unsigned FastAudioBuffer::size()
{
    return (tail - head) & mask;
}

bool FastAudioBuffer::full()
{
    return ((tail + 1) & mask) == head;
}
