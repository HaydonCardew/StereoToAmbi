/*
*  This file is part of StereoToAmbi.
*
*  StereoToAmbi is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*  
*  StereoToAmbi is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*  
*  You should have received a copy of the GNU General Public License
*  along with StereoToAmbi.  If not, see <https://www.gnu.org/licenses/>.
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
